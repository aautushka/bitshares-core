#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

#include <fc/io/json.hpp>
#include <fc/io/stdio.hpp>
#include <fc/network/http/server.hpp>
#include <fc/network/http/websocket.hpp>
#include <fc/rpc/cli.hpp>
#include <fc/rpc/http_api.hpp>
#include <fc/rpc/websocket_api.hpp>
#include <fc/smart_ref_impl.hpp>

#include <graphene/app/api.hpp>
#include <graphene/chain/protocol/protocol.hpp>
#include <graphene/egenesis/egenesis.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <graphene/wallet/wallet.hpp>
#include <graphene/wallet/appwallet.hpp>
#include <graphene/app/database_api.hpp>

#include <fc/interprocess/signals.hpp>
#include <boost/program_options.hpp>

#include <fc/log/console_appender.hpp>
#include <fc/log/file_appender.hpp>
#include <fc/log/logger.hpp>
#include <fc/log/logger_config.hpp>

#include <graphene/utilities/git_revision.hpp>

using namespace graphene::app;
using namespace graphene::chain;
using namespace graphene::utilities;
using namespace graphene::wallet;
using namespace std;

namespace graphene { namespace wallet {

// TODO
// 1. better error handling
// 2. no hand-crafted json strings
// 3. cleanup on exit
// 4. wallet.json hanling
// 5. raw transactions

class appwallet_impl
{
public:
    void disconnect()
    {
        wapiptr.reset();
        api_connection.reset();
    }

    void connect(std::string addr)
    {
        try
        {
            disconnect();
            auto con  = client.connect(addr);
            api_connection = std::make_shared<fc::rpc::websocket_api_connection>(*con, 255);
            auto remote_api = api_connection->get_remote_api<login_api>(1);
            auto database_api = api_connection->get_remote_api<graphene::app::database_api>(0);
            auto chain_id = database_api->get_chain_id();

            wallet_data wdata;
            wdata.ws_server = addr; 
            wdata.chain_id = chain_id;
            wapiptr = std::make_shared<wallet_api>( wdata, remote_api );

            fc::path wallet_file("/home/aautushka/projects/bitshares-core/node_addon/wallet.json");
            wapiptr->set_wallet_filename(wallet_file.generic_string());

            wapiptr->load_wallet_file();
        }
        catch (fc::exception& e)
        {
        }
    }

    std::string call(std::string request)
    {
        if (!wapiptr)
        {
            return "{\"jsonrpc\":\"2.0\",\"error\":{\"message\":\"network error\"}}";
        }

        try
        {
            fc::api<wallet_api> wapi(wapiptr);

            auto var = fc::json::from_string(request);
            const auto& var_obj = var.get_object();

            if( var_obj.contains( "method" ) )
            {
                const uint32_t DONT_KNOW_WHAT_THIS_CRAP_IS = 255;
                
                auto call = var.as<fc::rpc::request>(DONT_KNOW_WHAT_THIS_CRAP_IS);

                std::shared_ptr<fc::api_connection> ac(new fc::local_api_connection(DONT_KNOW_WHAT_THIS_CRAP_IS));
                fc::generic_api gapi(wapi, ac);
                auto result = gapi.call(call.method, call.params);

                auto reply = fc::json::to_string( fc::rpc::response( *call.id, result, "2.0" ) );
                return reply;
            }

            return "";
        }
        catch (fc::exception& e)
        {
            return std::string("{\"jsonrpc\":\"2.0\",\"error\":{\"message\":\"").append(e.to_string()).append("\"}}");
        }

        wapiptr->save_wallet_file();
        return "{\"jsonrpc\":\"2.0\",\"error\":{\"message\":\"unknown error\"}}";
    }

private:
    fc::http::websocket_client client;
    std::shared_ptr<fc::rpc::websocket_api_connection> api_connection;
    std::shared_ptr<wallet_api> wapiptr;
};

appwallet::appwallet()
{
    pimpl = new appwallet_impl;
}

appwallet::~appwallet()
{
    delete pimpl;
}

void appwallet::connect(std::string addr)
{
    pimpl->connect(addr);
}

std::string appwallet::test()
{
    return call("{\"jsonrpc\":\"2.0\",\"method\":\"get_global_properties\",\"params\":[],\"id\":1}");
}

std::string appwallet::call(std::string request)
{
    return pimpl->call(request);
}

} } // namespace graphene::wallet
