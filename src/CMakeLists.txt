cmake_minimum_required(VERSION 3.5)
project(tokensaleapp)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( tokensaleapp tokensaleapp tokensaleapp.cpp )
target_include_directories( tokensaleapp PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( tokensaleapp '${CMAKE_SOURCE_DIR}/../ricardian/' )
