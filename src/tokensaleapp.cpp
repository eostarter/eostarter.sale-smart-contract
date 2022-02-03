#include <tokensaleapp.hpp>

ACTION tokensaleapp::addpool( eosio::name    name,
                              string         info,
                              string         url,
                              eosio::name    owner,
                              eosio::name    token_contract,
                              symbol         token_symbol,
                              float          token_price,
                              asset          tokens_on_sale,
                              time_point_sec launch_date,
                              time_point_sec end_date,
                              float          project_immidiate_vesting,
                              uint16_t       project_vesting_days,
                              float          investor_immediate_vesting,
                              uint16_t       investor_vesting_days )
{
    // ask permission of owner account
    require_auth( owner );

    check( token_symbol == tokens_on_sale.symbol, "token symbol does not match" );
    // @todo: validate launch_date not to be lower than current date
    // @todo: validate end_date not to be lower than launch_date
    // @todo: validate info IPFS url
    // @todo: validate url format

    // save the pool as with PENDING_APPROVAL status
    pool_table _pools( get_self(), get_self().value );
    _pools.emplace( owner, [&]( auto &pool ) {
        pool.name = name;
        pool.info = info;
        pool.url = url;
        pool.owner = owner;
        pool.token_contract = token_contract;
        pool.token_symbol = token_symbol;
        pool.token_price = token_price;
        pool.tokens_on_sale = tokens_on_sale;
        pool.launch_date = launch_date;
        pool.end_date = end_date;
        pool.project_immidiate_vesting = project_immidiate_vesting;
        pool.project_vesting_days = project_vesting_days;
        pool.investor_immediate_vesting = investor_immediate_vesting;
        pool.investor_vesting_days = investor_vesting_days;
        pool.status = pool_status::PENDING_APPROVAL;
    } );
}

ACTION tokensaleapp::approvepool( eosio::name name )
{
    // ask permission self account
    require_auth( get_self() );

    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( name.value );

    // validate that the pool exists
    check( _pool != _pools.end(), "pool not found" );

    // validate that the pool have PENDING_APPROVAL status
    check( _pool->status == pool_status::PENDING_APPROVAL, "invalid status" );

    // change the pool status to PENDING_TOKEN_DEPOSIT
    _pools.modify( _pool, get_self(), [&]( auto &ref ) { ref.status = pool_status::PENDING_TOKEN_DEPOSIT; } );
}

void tokensaleapp::ontransfer( name from, name to, asset quantity, string memo )
{
    // skip transactions that are not for the contract
    if ( from == get_self() || to != get_self() )
    {
        return;
    }

    vector< string > params = get_params( memo );

    // allow tokens transfer as donation or to buy resources (CPU, NET, RAM)
    if ( params[0] == "donation" || params[0] == "resources" )
    {
        return;
    }

    // validate memo format
    check( params[0] == "pool" && params.size() == 2, "invalid memo" );

    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( eosio::name( params[1] ).value );

    // validate that the pool exists
    check( _pool != _pools.end(), "pool not found" );

    // validate that the pool have PENDING_TOKEN_DEPOSIT status
    check( _pool->status == pool_status::PENDING_TOKEN_DEPOSIT, "invalid status" );

    // validate quantity to be equal to tokens_on_sale
    check( _pool->tokens_on_sale == quantity, "invalid quantity" );

    // change the pool status
    _pools.modify( _pool, get_self(), [&]( auto &ref ) { ref.status = pool_status::READY_FOR_SALE; } );
}

vector< string > tokensaleapp::get_params( string memo )
{
    vector< string > params;
    string           param;
    size_t           pos = 0;
    string           delimiter = ":";

    while ( ( pos = memo.find( delimiter ) ) != string::npos )
    {
        param = memo.substr( 0, pos );
        params.push_back( param );
        memo.erase( 0, pos + delimiter.length() );
    }

    if ( memo.length() > 0 )
    {
        params.push_back( memo );
    }

    return params;
}

uint64_t tokensaleapp::to_uint64_t( string value )
{
    uint64_t a;
    char    *end;
    a = strtoull( value.c_str(), &end, 10 );

    return a;
}

ACTION tokensaleapp::clear()
{
    require_auth( get_self() );

    pool_table _pools( get_self(), get_self().value );

    // Delete all records in pools table
    auto pool = _pools.begin();
    while ( pool != _pools.end() )
    {
        pool = _pools.erase( pool );
    }
}
