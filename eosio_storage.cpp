#include <eosiolib/eosio.hpp>

using namespace eosio;

CONTRACT eosio_storage: public contract {
  public:
      using contract::contract;
      eosio_storage( name receiver, name code, datastream<const char*> ds )
         : contract(receiver, code, ds),
	posttab(receiver, receiver.value) {}


      ACTION set(name app, name account, name key, std::string value) {
         require_auth(account);
         print("create", key.value);
         uint64_t pk = posttab.available_primary_key();

         posttab.emplace( _self, [&]( auto& u ) {
             u.primary = pk;
             u.key = key;
             u.value = value;
             u.owner = account;
             u.app = app;
	     u.created = now();
         });

	 post_tables postusertab(_self, account.value);
         postusertab.emplace( _self, [&]( auto& u ) {
             u.primary = pk;
             u.key = key;
             u.value = value;
             u.owner = account;
             u.app = app;
	     u.created = now();
         });

	 post_tables postapptab(_self, name.value);
         postapptab.emplace( _self, [&]( auto& u ) {
             u.primary = pk;
             u.key = key;
             u.value = value;
             u.owner = account;
             u.app = app;
	     u.created = now();
         });
      }

      ACTION edit(name app, name account, uint64_t primary, std::string value) {
         require_auth(account);
	 auto itr = posttab.find(primary);
         eosio_assert(itr != posttab.end(), "not found");
         eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
         posttab.modify( itr, _self, [&]( auto& row ) {
           row.value = value;
           row.modified = now();
         });
	 post_tables postusertab(_self, account.value);
         postusertab.modify( postusertab.find(primary), _self, [&]( auto& row ) {
           row.value = value;
           row.modified = now();
         });
	 post_tables postapptab(_self, app.value);
         postapptab.modify( postapptab.find(primary), _self, [&]( auto& row ) {
           row.value = value;
           row.modified = now();
         });
      }
      
      ACTION erase(name app, name account, uint64_t primary) {
         require_auth(account);
	 auto itr = posttab.find(primary);
         eosio_assert(itr != posttab.end(), "not found");
         eosio_assert(itr->owner== account, "only the owner is allowed to erase this content");
	 posttab.erase(itr);
	 post_tables postusertab(_self, account.value);
	 postusertab.erase(postusertab.find(primary));
	 post_tables postapptab(_self, app.value);
	 postapptab.erase(postapptab.find(primary));
      }


      TABLE post_table {
         uint64_t primary;
         name key;
         name owner;
         name app;
         std::string value;
	 uint32_t created;
	 uint32_t modified;
         uint64_t primary_key() const { return primary; }
         uint64_t by_key() const { return key.value; }
         uint64_t by_owner() const { return owner.value; }
         uint64_t by_app() const { return app.value; }
      };

      typedef eosio::multi_index<"post"_n, post_table,  eosio::indexed_by<"key"_n, eosio::const_mem_fun<post_table, uint64_t, &post_table::by_key>>,  eosio::indexed_by<"ownerkey"_n, eosio::const_mem_fun<post_table, uint64_t, &post_table::by_owner>>,  eosio::indexed_by<"appkey"_n, eosio::const_mem_fun<post_table, uint64_t, &post_table::by_app>>> post_tables;

      using set_action   = action_wrapper<"set"_n, &eosio_storage::set>;	
      using edit_action   = action_wrapper<"edit"_n, &eosio_storage::edit>;
      using erase_action   = action_wrapper<"erase"_n, &eosio_storage::erase>;
      private:
        post_tables posttab;
};

EOSIO_DISPATCH( eosio_storage, (set)(edit)(erase) )
