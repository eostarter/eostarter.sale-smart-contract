#include <eosio/system.hpp>
#include <tokensaleapp.hpp>

ACTION tokensaleapp::adduser( name account, string username, bool verified, uint8_t role, name referral )
{
    // ask permission of self account
    require_auth( get_self() );

    // check if account is registered
    check( is_account( account ), "account is not registered" );

    // check if referral is registered
    check( is_account( referral ), "referral is not registered" );

    // account and referral should be different
    check( account != referral, "account is equal to referral" );

    // role must be PROJECT_OWNER or INVESTOR
    check( role == user_roles::PROJECT_OWNER || role == user_roles::INVESTOR, "invalid role" );

    // check if user is already registered
    user_table _users( get_self(), get_self().value );
    auto       _user = _users.find( account.value );
    check( _user == _users.end(), "user already exist" );

    // add the new user
    _users.emplace( get_self(), [&]( auto &user ) {
        user.account = account;
        user.username = username;
        user.verified = verified;
        user.role = role;
        user.referral = referral;
    } );
}

ACTION tokensaleapp::edituser( name account, bool verified )
{
    // ask permission of self account
    require_auth( get_self() );

    // check if user is already registered
    user_table _users( get_self(), get_self().value );
    auto       _user = _users.find( account.value );
    check( _user != _users.end(), "user not exist" );

    // update the user data
    _users.modify( _user, get_self(), [&]( auto &ref ) { ref.verified = verified; } );
}

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

    // validate token symbol
    check( token_symbol == tokens_on_sale.symbol, "token symbol does not match" );

    // check if owner is already registered and verified
    user_table _users( get_self(), get_self().value );
    auto       _user = _users.find( owner.value );
    check( _user != _users.end(), "owner not registered" );
    check( _user->verified, "owner not verified" );

    // pool name must be unique
    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( name.value );
    check( _pool == _pools.end(), "pool already exists" );

    // validate pool launch_date
    check( launch_date.sec_since_epoch() >= current_time_point().sec_since_epoch(),
           "launch_date must be higher than current time" );

    // validate pool end_date
    check( end_date.sec_since_epoch() >= current_time_point().sec_since_epoch(),
           "end_date must be higher than current time" );

    // @todo: validate info must be an IPFS url
    // @todo: validate url format

    // save the pool as with PENDING_APPROVAL status
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
        pool.status = pool_status::POOL_PENDING_APPROVAL;
    } );
}

ACTION tokensaleapp::approvepool( eosio::name name )
{
    // ask permission of self account
    require_auth( get_self() );

    // validate that the pool exists
    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( name.value );
    check( _pool != _pools.end(), "pool not found" );

    // validate that the pool have PENDING_APPROVAL status
    check( _pool->status == pool_status::POOL_PENDING_APPROVAL, "invalid status" );

    // change the pool status to PENDING_TOKEN_DEPOSIT
    _pools.modify( _pool, get_self(), [&]( auto &ref ) { ref.status = pool_status::POOL_PENDING_TOKEN_DEPOSIT; } );
}

ACTION tokensaleapp::startsale( eosio::name name )
{
    // validate that the pool exists
    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( name.value );
    check( _pool != _pools.end(), "pool not found" );

    // ask permission of self account or owner account
    check( has_auth( get_self() ) || has_auth( _pool->owner ), "missing required authorization" );

    // validate that the pool have READY_FOR_SALE status
    check( _pool->status == pool_status::POOL_READY_FOR_SALE, "invalid status" );

    // validate pool launch_date
    check( _pool->launch_date.sec_since_epoch() <= current_time_point().sec_since_epoch(),
           "launch_date must be lower than current time" );

    // validate pool end_date
    check( _pool->end_date.sec_since_epoch() >= current_time_point().sec_since_epoch(),
           "end_date must be higher than current time" );

    // change the pool status to ACTIVE_SALE
    _pools.modify( _pool, get_self(), [&]( auto &ref ) { ref.status = pool_status::POOL_ACTIVE_SALE; } );
}

ACTION tokensaleapp::subscribe( name account, name pool )
{
    // ask permission of user account
    require_auth( account );

    // validate that the pool exists with status ACTIVE_SALE
    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( pool.value );
    check( _pool != _pools.end(), "pool not found" );
    check( _pool->status == pool_status::POOL_ACTIVE_SALE, "invalid pool status" );

    // check if user account is already subscribed
    subscription_table _subscriptions( get_self(), pool.value );
    auto               _subscription = _subscriptions.find( account.value );
    check( _subscription == _subscriptions.end(), "account already subscribed" );

    // save the pool as with PENDING_APPROVAL status
    _subscriptions.emplace( account, [&]( auto &subscription ) {
        subscription.account = account;
        subscription.status = subscription_status::SUBSCRIPTION_PENDING_APPROVAL;
    } );
}

ACTION tokensaleapp::approvesubsc( name account, name pool, float max_allocation )
{
    // ask permission of self account
    require_auth( get_self() );

    // validate that the pool exists with status ACTIVE_SALE
    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( pool.value );
    check( _pool != _pools.end(), "pool not found" );
    check( _pool->status == pool_status::POOL_ACTIVE_SALE, "invalid pool status" );

    // validate that the user account it's already subscribed
    subscription_table _subscriptions( get_self(), pool.value );
    auto               _subscription = _subscriptions.find( account.value );
    check( _subscription != _subscriptions.end(), "account not subscribed" );

    // change the pool status to APPROVED
    _subscriptions.modify( _subscription, get_self(), [&]( auto &ref ) {
        ref.max_allocation = max_allocation;
        ref.status = subscription_status::SUBSCRIPTION_APPROVED;
    } );
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

    // process pool token deposit
    if ( params[0] == "pool" && params.size() == 2 )
    {
        tokendeposit( name( params[1] ), quantity );
        return;
    }

    // process pool contibution
    if ( params[0] == "contribution" && params.size() == 2 )
    {
        poolcontribution( from, name( params[1] ), quantity );
        return;
    }

    check( false, "invalid memo" );
}

void tokensaleapp::tokendeposit( name pool, asset quantity )
{
    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( pool.value );

    // validate that the pool exists
    check( _pool != _pools.end(), "pool not found" );

    // validate that the pool have PENDING_TOKEN_DEPOSIT status
    check( _pool->status == pool_status::POOL_PENDING_TOKEN_DEPOSIT, "invalid status" );

    // validate the token contract
    check( _pool->token_contract == get_first_receiver(), "invalid contract" );

    // validate quantity to be equal to tokens_on_sale
    check( _pool->tokens_on_sale == quantity, "invalid quantity" );

    // change the pool status
    _pools.modify( _pool, get_self(), [&]( auto &ref ) { ref.status = pool_status::POOL_READY_FOR_SALE; } );
}

void tokensaleapp::poolcontribution( name from, name pool, asset quantity )
{
    // validate the token contract
    check( SUPPORTED_TOKEN_CONTRACT == get_first_receiver(), "invalid contract" );

    // validate the token symbol
    check( SUPPORTED_TOKEN_SYMBOL == quantity.symbol, "invalid symbol" );

    pool_table _pools( get_self(), get_self().value );
    auto       _pool = _pools.find( pool.value );

    // validate that the pool exists
    check( _pool != _pools.end(), "pool not found" );

    // validate that the pool have ACTIVE_SALE status
    check( _pool->status == pool_status::POOL_ACTIVE_SALE, "invalid pool status" );

    subscription_table _subscriptions( get_self(), pool.value );
    auto               _subscription = _subscriptions.find( from.value );

    // validate that the user account it's already subscribed
    check( _subscription != _subscriptions.end(), "account not subscribed" );

    // validate that the subscription have APPROVED status
    check( _subscription->status == subscription_status::SUBSCRIPTION_APPROVED, "invalid subscription status" );

    asset prev_contribution;

    if ( _subscription->contribution.is_valid() )
    {
        prev_contribution = _subscription->contribution;
    }
    else
    {
        prev_contribution = asset( 0, SUPPORTED_TOKEN_SYMBOL );
    }

    // calculate the new stats
    asset new_contribution = prev_contribution + quantity;
    auto  temp = (int64_t)( (float)new_contribution.amount / 10000 / _pool->token_price * 10000 );
    asset new_balance = asset( temp, _pool->token_symbol );
    auto  new_current_allocation = (float)new_balance.amount / (float)_pool->tokens_on_sale.amount * 100;

    // current_allocation must be lower or equal to max_allocation
    check( new_current_allocation <= _subscription->max_allocation, "over max allocation" );

    // update the subscription prev_contribution and allocation
    _subscriptions.modify( _subscription, get_self(), [&]( auto &ref ) {
        ref.contribution = new_contribution;
        ref.balance = new_balance;
        ref.current_allocation = new_current_allocation;
    } );
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

ACTION tokensaleapp::clear()
{
    require_auth( get_self() );

    // Delete all records in user table
    user_table _users( get_self(), get_self().value );
    auto       user = _users.begin();

    while ( user != _users.end() )
    {
        user = _users.erase( user );
    }

    // Delete all records in pool table
    pool_table _pools( get_self(), get_self().value );
    auto       pool = _pools.begin();

    while ( pool != _pools.end() )
    {
        // Delete all records in subscription table
        subscription_table _subscriptions( get_self(), pool->name.value );
        auto               subscription = _subscriptions.begin();

        while ( subscription != _subscriptions.end() )
        {
            subscription = _subscriptions.erase( subscription );
        }

        pool = _pools.erase( pool );
    }
}
