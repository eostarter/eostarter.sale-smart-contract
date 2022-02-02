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
    ACTION approvepool( eosio::name name );

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

    uint64_t to_uint64_t( string value );

  private:
    enum pool_status : uint8_t
    {
        PENDING_APPROVAL = 1,
        PENDING_TOKEN_DEPOSIT = 2,
        READY_FOR_SALE = 3,
        ACTIVE_SALE = 4,
        COMPLETED_SALE = 5
    };

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
    typedef multi_index< eosio::name( "pools" ), pool > pool_table;
};