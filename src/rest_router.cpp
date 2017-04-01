
// Used to enable debug printouts.
const bool debug = true;


#include "rest_router.hpp"

#include <json.hpp>
#include <pistache/mime.h>
#include <pistache/router.h>
#include "battlesnake_api.hpp"

    void from_json(const nlohmann::json& j, Point& p) {
        p.x = j["x"];
        p.y = j["y"];
    }

    void from_json(const nlohmann::json& j, Snake& s) {
        s.name = j["name"];
        s.taunt = j["taunt"];
        s.id = j["id"];
        s.health_points = j["health_points"];
        //s.coords = j["coords"].get<Points>;
    }

    void to_json(nlohmann::json& j, const Move& m) {
    switch(m) {
    case Move::up:
        j = "up";
        break;
    case Move::left:
        j = "left";
        break;
    case Move::down:
        j = "down";
        break;
    case Move::right:
        j = "right";
        break;
    }
    }


Net::Rest::Router get_router() {
    Net::Rest::Router router;

    // Handle /start request.
    Net::Rest::Routes::Post(router, "/start", [] (const Net::Rest::Request& request, Net::Http::ResponseWriter response) {
        nlohmann::json req = nlohmann::json::parse(request.body());

        auto game_id = req["game_id"].get<std::string>();
        auto height = req["height"].get<int>();
        auto width = req["width"].get<int>();

        // battlesnake_start() is implemented by the user.
        auto response_body = battlesnake_start(game_id, height, width);

        response.send(Net::Http::Code::Ok, response_body.dump(4), MIME(Application, Json));
        return Net::Rest::Route::Result::Ok;
    });


    // Handle /move request.
    Net::Rest::Routes::Post(router, "/move", [] (const Net::Http::Request& request, Net::Http::ResponseWriter response) {
        nlohmann::json req = nlohmann::json::parse(request.body());

        if (debug) {
            std::cout << "*************** MOVE ******************" << std::endl;
            std::cout << req.dump(4) << std::endl;
        }

        const Snakes snakes = req["snakes"];
        const std::string you_uuid = req["you"];
        size_t you = -1;
        for (size_t i = 0; i < snakes.size(); ++i) {
            std::cout << "+++ " << snakes[i].id << std::endl;
            if (snakes[i].id == you_uuid) {
                you = i;
                break;
            }
        }
        assert(you >= 0);

        // battlesnake_start() is implemented by the user.
        auto move = battlesnake_move(
                req["food"],
                req["game_id"],
                req["height"],
                req["width"],
                snakes,
                req["dead_snakes"],
                you);

        nlohmann::json response_body = {{"move", move.move}};

        if (!move.taunt.empty()) {
            response_body["taunt"] = move.taunt;
        }

        if (debug) {
            std::cout << "RESPONSE:" << std::endl;
            std::cout << req.dump(4) << std::endl << std::endl;
        }

        response.send(Net::Http::Code::Ok, response_body.dump(4), MIME(Application, Json));
        return Net::Rest::Route::Result::Ok;
    });


    return router;
}
