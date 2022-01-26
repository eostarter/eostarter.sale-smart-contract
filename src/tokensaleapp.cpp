#include <tokensaleapp.hpp>

ACTION tokensaleapp::addpool( name           owner,
                              name           token_contract,
                              symbol         token_symbol,
                              uint8_t        token_price,
                              time_point_sec launch_date,
                              time_point_sec end_date,
                              uint8_t        immidiate_vesting,
                              uint8_t        vesting_days,
                              uint8_t        investor_immediate_vesting,
                              uint8_t        investor_vesting_days )
{
    // ask permission of owner account
    require_auth( owner );

    // save the pool as a draft
    pool_table _pools( get_self(), get_self().value );
    _pools.emplace( owner, [&]( auto &pool ) {
        pool.id = _pools.available_primary_key();
        pool.owner = owner;
        pool.token_contract = token_contract;
        pool.token_symbol = token_symbol;
        pool.token_price = token_price;
        pool.launch_date = launch_date;
        pool.end_date = end_date;
        pool.immidiate_vesting = immidiate_vesting;
        pool.vesting_days = vesting_days;
        pool.investor_immediate_vesting = investor_immediate_vesting;
        pool.investor_vesting_days = investor_vesting_days;
        pool.status = pool_status::PENDING_APPROVAL;
    } );
}

ACTION tokensaleapp::approvepool( uint64_t id )
{
    // ask permission self account
    require_auth( get_self() );

    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( id );

    // validate that the pool exists
    check( _pool != _pools.end(), "pool not found" );

    // validate that the pool have PENDING_APPROVAL status
    check( _pool->status == pool_status::PENDING_APPROVAL, "invalid status" );

    // change the pool status
    _pools.modify( _pool, get_self(), [&]( auto &ref ) { ref.status = pool_status::ACTIVE; } );
}

ACTION tokensaleapp::hi( name from, string message )
{

    require_auth( from );

    // Init the _message table
    messages_table _messages( get_self(), get_self().value );

    // Find the record from _messages table
    auto msg_itr = _messages.find( from.value );
    if ( msg_itr == _messages.end() )
    {
        // Create a message record if it does not exist
        _messages.emplace( from, [&]( auto &msg ) {
            msg.user = from;
            msg.text = message;
        } );
    }
    else
    {
        // Modify a message record if it exists
        _messages.modify( msg_itr, from, [&]( auto &msg ) { msg.text = message; } );
    }
}

ACTION tokensaleapp::clear()
{
    require_auth( get_self() );

    messages_table _messages( get_self(), get_self().value );

    // Delete all records in _messages table
    auto msg_itr = _messages.begin();
    while ( msg_itr != _messages.end() )
    {
        msg_itr = _messages.erase( msg_itr );
    }

    pool_table _pools( get_self(), get_self().value );

    // Delete all records in pools table
    auto pool = _pools.begin();
    while ( pool != _pools.end() )
    {
        pool = _pools.erase( pool );
    }
}
