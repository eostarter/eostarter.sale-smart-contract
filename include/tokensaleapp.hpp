#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT tokensaleapp : public contract {
  public:
    using contract::contract;

    ACTION hi(name from, string message);
    ACTION clear();

  private:
    TABLE messages {
      name    user;
      string  text;
      auto primary_key() const { return user.value; }
    };
    typedef multi_index<name("messages"), messages> messages_table;
};
