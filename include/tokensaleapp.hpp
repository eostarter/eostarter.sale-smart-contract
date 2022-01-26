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

    ACTION hi( name from, string message );
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
    ACTION addpool( name           owner,
                    name           token_contract,
                    symbol         token_symbol,
                    uint8_t        token_price,
                    time_point_sec launch_date,
                    time_point_sec end_date,
                    uint8_t        immidiate_vesting,
                    uint8_t        vesting_days,
                    uint8_t        investor_immediate_vesting,
                    uint8_t        investor_vesting_days );

    /**
     * approve pool
     *
     * This action allow approve a pool
     *
     * @param id
     *
     * @return no return value.
     */
    ACTION approvepool( uint64_t id );

  private:
    enum pool_status : uint8_t
    {
        PENDING_APPROVAL = 1,
        ACTIVE = 2,
    };

    TABLE pool
    {
        uint64_t       id;
        name           owner;
        uint8_t        status;
        name           token_contract;
        symbol         token_symbol;
        uint8_t        token_price;
        time_point_sec launch_date;
        time_point_sec end_date;
        uint8_t        immidiate_vesting;
        uint8_t        vesting_days;
        uint8_t        investor_immediate_vesting;
        uint8_t        investor_vesting_days;

        auto primary_key() const
        {
            return id;
        }
    };
    typedef multi_index< name( "pools" ), pool > pool_table;

    TABLE messages
    {
        name   user;
        string text;
        auto   primary_key() const
        {
            return user.value;
        }
    };
    typedef multi_index< name( "messages" ), messages > messages_table;
};