#pragma once 

namespace graphene { namespace wallet {

class appwallet_impl;

class appwallet
{
public:
   appwallet();
   ~appwallet();

   void connect(std::string addr);
   std::string test();
   std::string call(std::string request);

private:
   appwallet_impl* pimpl; 
};

} } // namespace graphene::wallet
