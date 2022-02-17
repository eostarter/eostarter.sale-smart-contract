/*
 *
 * @author  EOSCostaRica.io [ https://eoscostarica.io ]
 *
 * @section DESCRIPTION
 *  Header file for the declaration of all functions
 *
 *  GitHub: https://github.com/eostarter/eostarter.sale-smart-contract
 *
 */
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT tokensaleapp : public contract
{
  public:
    using contract::contract;

    ACTION clear();

    /**
     * add user
     *
     * This action allow to add an user
     *
     * @param account
     * @param username
     * @param verified
     * @param role
     * @param referral
     *
     * @return no return value.
     */
    ACTION adduser( name account, string username, bool verified, uint8_t role, name referral );

    /**
     * edit user
     *
     * This action allow to edit an user
     *
     * @param account
     * @param verified
     *
     * @return no return value.
     */
    ACTION edituser( name account, bool verified );

    /**
     * Add pool
     *
     * This action allow add a new pool
     *
     * @param owner
     * @param token_contract
     * @param token_symbol
     * @param token_price
     * @param launch_date
     * @param end_date
     * @param immidiate_vesting
     * @param vesting_days
     * @param investor_immediate_vesting
     * @param investor_vesting_days
     *
     * @return no return value.
     */
    ACTION addpool( eosio::name    name,
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
                    uint16_t       investor_vesting_days );

    /**
     * approve pool
     *
     * This action allow approve a pool
     *
     * @param name
     *
     * @return no return value.
     */
    ACTION approvepool( name name );

    /**
     * start sale
     *
     * This action start the pool sale
     *
     * @param name
     *
     * @return no return value.
     */
    ACTION startsale( name name );

    /**
     * end sale
     *
     * This action end the pool sale
     *
     * @param name
     *
     * @return no return value.
     */
    ACTION endsale( name name );

    /**
     * subscribe
     *
     * This action subscribe an user to the pool
     *
     * @param name
     *
     * @return no return value.
     */
    ACTION subscribe( name account, name pool );

    /**
     * approve subscription
     *
     * This action approve subscription of an user to a pool
     *
     * @param name
     *
     * @return no return value.
     */
    ACTION approvesubsc( name account, name pool, asset max_allocation );

    /**
     * On Transfer
     *
     * Handle deposits for pools
     *
     * @param from
     * @param to
     * @param quantity
     * @param memo
     *
     * @return no return value.
     */
    [[eosio::on_notify( "*::transfer" )]] void ontransfer( name from, name to, asset quantity, string memo );

    vector< string > get_params( string memo );

    void tokendeposit( name pool, asset quantity );

    void poolcontribution( name from, name pool, asset quantity );

  private:
    enum user_roles : uint8_t
    {
        PROJECT_OWNER = 1,
        INVESTOR = 2,
    };

    enum pool_status : uint8_t
    {
        POOL_PENDING_APPROVAL = 1,
        POOL_PENDING_TOKEN_DEPOSIT = 2,
        POOL_READY_FOR_SALE = 3,
        POOL_ACTIVE_SALE = 4,
        POOL_CLAIM_IN_PROGRESS = 5,
        POOL_COMPLETED_SALE = 6,
        POOL_REJECTED = 7
    };

    enum subscription_status : uint8_t
    {
        SUBSCRIPTION_PENDING_APPROVAL = 1,
        SUBSCRIPTION_APPROVED = 2,
        SUBSCRIPTION_CLAIM_IN_PROGRESS = 3,
        SUBSCRIPTION_PAID = 4,
        SUBSCRIPTION_REJECTED = 0
    };

    // @todo: allow different token contract per pool as as buy currency
    const name SUPPORTED_TOKEN_CONTRACT = name( "eosio.token" );

    // @todo: allow different token symbol per pool as as buy currency
    const symbol SUPPORTED_TOKEN_SYMBOL = symbol( "EOS", 4 );

    TABLE user
    {
        name    account;
        string  username;
        bool    verified;
        uint8_t role;
        name    referral;

        auto primary_key() const
        {
            return account.value;
        }
    };
    typedef multi_index< eosio::name( "user" ), user > user_table;

    TABLE pool
    {
        eosio::name    name;
        string         info;
        string         url;
        eosio::name    owner;
        eosio::name    token_contract;
        symbol         token_symbol;
        float          token_price;
        asset          tokens_on_sale;
        time_point_sec launch_date;
        time_point_sec end_date;
        float          project_immidiate_vesting;
        uint16_t       project_vesting_days;
        float          investor_immediate_vesting;
        uint16_t       investor_vesting_days;
        uint8_t        status;

        auto primary_key() const
        {
            return name.value;
        }
    };
    typedef multi_index< eosio::name( "pool" ), pool > pool_table;

    TABLE subscription
    {
        name           account;
        asset          max_allocation;
        asset          contribution;
        asset          total_balance;
        asset          remaining_balance;
        asset          claimed_balance;
        time_point_sec last_claim;
        uint8_t        status;

        auto primary_key() const
        {
            return account.value;
        }
    };
    typedef multi_index< eosio::name( "subscription" ), subscription > subscription_table;
};