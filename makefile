
build-contracts:
	@mkdir -p build
	@rm -rf build/*
	@cd build && cmake .. && make

deploy-contracts:
	$(eval -include .env)
	@cleos wallet unlock --name $(CONTRACTS_TOKENSALEAPP_ACCOUNT) --password $(CONTRACTS_TOKENSALEAPP_PASSWORD) || echo ""
	@cleos -u $(CONTRACTS_CHAIN_ENDPOINT) set code $(CONTRACTS_TOKENSALEAPP_ACCOUNT) ./build/eostarter.sale-smart-contract/tokensaleapp.wasm || echo ""
	@cleos -u $(CONTRACTS_CHAIN_ENDPOINT) set abi $(CONTRACTS_TOKENSALEAPP_ACCOUNT) ./build/eostarter.sale-smart-contract/tokensaleapp.abi || echo ""
	@cleos wallet lock --name $(CONTRACTS_TOKENSALEAPP_ACCOUNT)
