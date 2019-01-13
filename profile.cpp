#include <eosiolib/eosio.hpp>

using namespace eosio;

CONTRACT profile : public contract {
  public:
      using contract::contract;
      profile( name receiver, name code, datastream<const char*> ds )
         : contract(receiver, code, ds),
	profiletab(receiver, receiver.value),
	posttab(receiver, receiver.value),
	publictab(receiver, receiver.value) {}

      ACTION setprofile(name account, name key, std::string value) {
         require_auth(account);

          auto idx = profiletab.get_index<"ownerkey"_n>();
          auto itr = idx.lower_bound(account.value);
	  while (itr != idx.end() && itr->key.value != key.value && itr->owner.value != account.value) {itr++;}
          if ( itr == idx.end() ) {
            print("create");
            profiletab.emplace( _self, [&]( auto& u ) {
              u.primary = profiletab.available_primary_key();
              u.key = key;
              u.value = value;
              u.owner = account;
              u.created = now();
            });
          } else {
            eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
            profiletab.modify( *itr, _self, [&]( auto& row ) {
              row.value = value;
	      row.modified = now();
            });
          }
      }

      ACTION eraseprofile(name account, name key) {
         require_auth(account);

          auto idx = profiletab.get_index<"ownerkey"_n>();
          auto itr = idx.lower_bound(account.value);
	  while (itr != idx.end() && itr->key.value != key.value) {itr++;}
          eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
          eosio_assert(itr != idx.end(), "not found");
	  profiletab.erase(*itr);
      }

      ACTION setpublic(name account, name key, std::string value) {
         require_auth(account);
          auto idx = publictab.get_index<"key"_n>();
          auto itr = idx.lower_bound(key.value);
	  print(itr->key.value," ");
	  print(key.value);
          if ( itr == idx.end() || itr->key.value != key.value ) {
              publictab.emplace( _self, [&]( auto& u ) {
                  u.primary = publictab.available_primary_key();
                  u.key = key;
                  u.value = value;
                  u.owner = account;
	          u.created = now();
              });
          } else {
	      eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
              publictab.modify( *itr, _self, [&]( auto& row ) {
                row.value = value;
                row.modified = now();
              });
          }
      }

      ACTION erasepublic(name account, name key) {
         require_auth(account);

          auto idx = publictab.get_index<"key"_n>();
          auto itr = idx.lower_bound(account.value);
	  while (itr != idx.end() && itr->key.value != key.value) {itr++;}
          eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
          eosio_assert(itr != idx.end(), "not found");
	  publictab.erase(*itr);
      }

      ACTION setpost(name account, name key, std::string value) {
         require_auth(account);
         print("create", key.value);

         posttab.emplace( _self, [&]( auto& u ) {
             u.primary = posttab.available_primary_key();
             u.key = key;
             u.value = value;
             u.owner = account;
	     u.created = now();
         });
      }

      ACTION editpost(name account, uint64_t primary, std::string value) {
         require_auth(account);
	 auto itr = posttab.find(primary);
         eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
         posttab.modify( itr, _self, [&]( auto& row ) {
           row.value = value;
           row.modified = now();
         });
      }
      
      ACTION erasepost(name account, name key) {
         require_auth(account);

          auto idx = posttab.get_index<"key"_n>();
          auto itr = idx.lower_bound(account.value);
	  while (itr != idx.end() && itr->key.value != key.value) {itr++;}
          eosio_assert(itr->owner.value == account.value, "only the owner is allowed to modify this content");
          eosio_assert(itr != idx.end(), "not found");
	  posttab.erase(*itr);
      }

      TABLE profile_table {
         uint64_t primary;
         name key;
         name owner;
         std::string value;
	 uint32_t created;       
	 uint32_t modified;       
         uint64_t primary_key() const { return primary; }
         uint64_t by_key() const { return key.value; }
         uint64_t by_owner() const { return owner.value; }
      };

      TABLE public_table {
         uint64_t primary;
         name key;
	 name owner;
         std::string value;
	 uint32_t created;       
	 uint32_t modified;       
         uint64_t primary_key() const { return primary; }
         uint64_t by_key() const { return key.value; }
      };

      TABLE post_table {
         uint64_t primary;
         name key;
         name owner;
         std::string value;
	 uint32_t created;       
	 uint32_t modified;       
         uint64_t primary_key() const { return primary; }
         uint64_t by_key() const { return key.value; }
      };

      typedef eosio::multi_index<"profiles"_n, profile_table,  eosio::indexed_by<"key"_n, eosio::const_mem_fun<profile_table, uint64_t, &profile_table::by_key>>,  eosio::indexed_by<"ownerkey"_n, eosio::const_mem_fun<profile_table, uint64_t, &profile_table::by_owner>>> profile_tables;
      typedef eosio::multi_index<"public"_n, public_table,  eosio::indexed_by<"key"_n, eosio::const_mem_fun<public_table, uint64_t, &public_table::by_key>>> public_tables;
      typedef eosio::multi_index<"post"_n, post_table,  eosio::indexed_by<"key"_n, eosio::const_mem_fun<post_table, uint64_t, &post_table::by_key>>> post_tables;

      using setprofile_action   = action_wrapper<"setprofile"_n, &profile::setprofile>;
      using setpublic_action   = action_wrapper<"setpublic"_n, &profile::setpublic>;
      using setpost_action   = action_wrapper<"setpost"_n, &profile::setpost>;
      private:
        profile_tables profiletab;
        public_tables publictab;
        post_tables posttab;
};

EOSIO_DISPATCH( profile, (setprofile)(setpublic)(setpost) )
