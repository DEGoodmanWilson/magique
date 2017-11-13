#include <iostream>

#include <luna/luna.h>
#include "logger.h"
#include "catalog.h"
#include "collection.h"
#include "card.h"

using namespace magique;

int main()
{
    // set up the loggers
    luna::set_access_logger(access_logger);
    luna::set_error_logger(error_logger);

    // determine which port to run on, default to 8080
    auto port = 8080;
    if (auto port_str = std::getenv("PORT"))
    {
        try
        {
            port = std::atoi(port_str);
        }
        catch (const std::invalid_argument &e)
        {
            error_logger(luna::log_level::FATAL, "Invalid port specified in env $PORT.");
            return 1;
        }
        catch (const std::out_of_range &e)
        {
            error_logger(luna::log_level::FATAL, "Port specified in env $PORT is too large.");
            return 1;
        }
    }


    // fire up a catalog
    catalog master_catalog{"mtg.json"};

    // add endpoints
    luna::router api{"/"};
    api.set_mime_type("text/json");

    // For now, let's just define them inline, we can move them out later.
    api.handle_request(luna::request_method::GET, R"(/(.+))", [&](luna::request request) -> luna::response
    {
        try
        {
            auto card = master_catalog.at(request.matches[1]);
            nlohmann::json j = card;
            return {j.dump()};
        }
        catch(...)
        {
            return {404};
        }
    });


    // launch server
    luna::server server;

    server.add_router(api);

    if (!server.start(port))
    {
        return 1;
    }


    return 0;
}