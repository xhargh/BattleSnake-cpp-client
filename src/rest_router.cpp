
// Used to enable debug printouts.
const bool debug = false;
const bool print_move = true;


#include "rest_router.hpp"
#include "json.hpp"
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
        s.coords = j["coords"].get<Points>();

        {
            const nlohmann::json taunt = j["taunt"];
            if (taunt.is_string()) {
                s.taunt = taunt;
            }
        }
    }

    void to_json(nlohmann::json& j, const Direction& m) {
    switch(m) {
    case Direction::up:
        j = "up";
        break;
    case Direction::left:
        j = "left";
        break;
    case Direction::down:
        j = "down";
        break;
    case Direction::right:
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
            auto width = req["width"].get<int>();
            auto height = req["height"].get<int>();

            // battlesnake_start() is implemented by the user.
            auto response_body = battlesnake_start(game_id, width, height);

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
            size_t my_snake_index = -1;
            for (size_t i = 0; i < snakes.size(); ++i) {
                if (snakes[i].id == you_uuid) {
                    my_snake_index = i;
                    break;
                }
            }
            if (my_snake_index < 0) {
                throw std::runtime_error("Could not find you uuid in list of snakes.");
            }



            // battlesnake_start() is implemented by the user.
            auto move = battlesnake_move(
                    req["game_id"].get<std::string>(),
                    req["width"].get<int>(),
                    req["height"].get<int>(),
                    req["food"].get<Points>(),
                    snakes,
                    req["dead_snakes"].get<Snakes>(),
                    my_snake_index);

            response_body = nlohmann::json::object();
            response_body["move"] = move.direction;

            if (!move.taunt.empty()) {
                response_body["taunt"] = move.taunt;
            }

            if (debug) {
                std::cout << "RESPONSE:" << std::endl;
            }
            if (debug || print_move) {
                std::cout << response_body << std::endl << std::endl;
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
