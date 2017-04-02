
// Used to enable debug printouts.
const bool debug = true;


#include "rest_router.hpp"

#include <json.hpp>
#include <pistache/mime.h>
#include <pistache/router.h>
#include "battlesnake_api.hpp"

    void from_json(const nlohmann::json& j, Point& p) {
        p.x = j[0];
        p.y = j[1];
    }

    void from_json(const nlohmann::json& j, Snake& s) {
        s.name = j["name"].get<std::string>();
        s.id = j["id"].get<std::string>();
        s.health_points = j["health_points"].get<int>();
        //s.coords = j["coords"].get<Points>;

        {
            const nlohmann::json taunt = j["taunt"];
            if (taunt.is_string()) {
                s.taunt = taunt;
            }
        }
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
        try {
            nlohmann::json req = nlohmann::json::parse(request.body());

            auto game_id = req["game_id"].get<std::string>();
            auto height = req["height"].get<int>();
            auto width = req["width"].get<int>();

            // battlesnake_start() is implemented by the user.
            auto response_body = battlesnake_start(game_id, height, width);

            response.send(Net::Http::Code::Ok, response_body.dump(4), MIME(Application, Json));

        } catch(std::exception& e) {
            std::cerr << "ERROR in /start: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "ERROR in /start: Unknown exception caught." << std::endl;
        }

        return Net::Rest::Route::Result::Ok;
    });


    // Handle /move request.
    Net::Rest::Routes::Post(router, "/move", [] (const Net::Rest::Request& request, Net::Http::ResponseWriter response) {
        nlohmann::json response_body;
        try {
            nlohmann::json req = nlohmann::json::parse(request.body());

            if (debug) {
                std::cout << "*************** MOVE ******************" << std::endl;
                std::cout << req.dump(4) << std::endl;
            }

            const Snakes snakes = req["snakes"].get<Snakes>();
            const std::string you_uuid = req["you"];
            size_t you = -1;
            for (size_t i = 0; i < snakes.size(); ++i) {
                std::cout << "+++ " << snakes[i].id << std::endl;
                if (snakes[i].id == you_uuid) {
                    you = i;
                    break;
                }
            }
            if (you < 0) {
                throw std::runtime_error("Could not find you uuid in list of snakes.");
            }



            // battlesnake_start() is implemented by the user.
            auto move = battlesnake_move(
                    req["food"].get<Points>(),
                    req["game_id"].get<std::string>(),
                    req["height"].get<int>(),
                    req["width"].get<int>(),
                    snakes,
                    req["dead_snakes"].get<Snakes>(),
                    you);

            response_body = nlohmann::json::object();
            response_body["move"] = move.move;

            if (!move.taunt.empty()) {
                response_body["taunt"] = move.taunt;
            }

            if (debug) {
                std::cout << "RESPONSE:" << std::endl;
                std::cout << response_body.dump(4) << std::endl << std::endl;
            }


        } catch(std::exception& e) {
            std::cerr << "ERROR in /move: " << e.what() << std::endl;

            // Make a stupid move anyway.
            response_body = {
                    { "move", "right" },
                    { "taunt", std::string("Ouch!! ") + e.what() }
            };
        } catch (...) {
            std::cerr << "ERROR in /move: Unknown exception caught." << std::endl;
        }

        response.send(Net::Http::Code::Ok, response_body.dump(4), MIME(Application, Json));
        return Net::Rest::Route::Result::Ok;
    });


    return router;
}
