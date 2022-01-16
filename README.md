<p align="center">
	<a href="https://eostarter.org">
		<img src="./docs/img/logo-words.png" width="300">
	</a>
</p>

# EOS Token Sale Contract

![](https://img.shields.io/github/license/eostarter/eostarter.sale-smart-contract) ![](https://img.shields.io/badge/code%20style-standard-brightgreen.svg) ![](https://img.shields.io/badge/%E2%9C%93-collaborative_etiquette-brightgreen.svg) ![](https://img.shields.io/twitter/follow/eostarter1.svg?style=social&logo=twitter) ![](https://img.shields.io/github/forks/eostarter/eostarter.sale-smart-contract?style=social)

EOStarter is developing a Token Sale Smart Contract system that allows for the crowd-sourcing of projects through a token sale that can be configured with different parameters like staking capabilities, whitelisting requirements, token vesting, etc.

You can read about the full specs of these  Smart Contracts here:

[TOKENSALE PLATFORM - SCOPE OF WORK](https://medium.com/eostarter/eostarter-tokensale-platform-scope-of-work-4cb153c33ab6)


# Project Description
These contracts are intended to support a decentralized Token Sale platform, which allows users to set up a token sale, determine the token sale parameters, and deploy their sale onto the EOS Mainnet.

The token sale platform is a decentralized application based on open-source smart contracts that any project can use to sell its tokens.

The Web Application repository these smart contracts are designed for can be found at the [EOS Token Sale Platform GitHub Repo](https://github.com/eostarter/eostarter.sale-smart-contract)

## Situation and Approach

### User Roles
- Project Owner 
- Investor 
- Application Backend Service


### User Flow
1. A token contract generator will provide project owners with a copy of the `eosio.token` contract 
1. Project owner deploys contract , created and issues tokens.
1. Project Owner Creates a pool and sets parameters
1. Application Backend Service approves pool
1. Project Owner deposits token to smart contract
1. Token sale goes live

1. Investor applies for project pool 
1. Application Backend Service whitelists investor account
1. Investor must agree to terms
1. Investor deposits EOS 
1. Investor receives equivalent amount in project tokens

## Data Model

<p align="center">
	<img src="./docs/img/data-model.png" >
</p>

## Smart Contract Actions 

| User Role | Action | Description | Pre-Conditions | Post-Conditions |
|---|---|---|---|---|
| Project Owner | addpool | create a new pool | Account must exist and be verified | pool is enabled | pool is enabled 
| Project Owner | deposit | deposit tokens to pool | withdraw EOS tokens from pool | pool must exist | pool is funded
| Project Owner | claimfunds  | withdraw EOS tokens from pool | pool must exist and vesting schedule met | project is funded   |
|  |   |   |   |   |
| Investor | subscribe | Apply to a pool | Investor must be qualified and agree to terms | User is subscribed to a pool |
| Investor | contribute  | Contribute to a pool  | Pool must exist and user is verified | User can invest in a pool |
| Investor | claim  | Claim project tokens from a pool | Account must have contributed | User no longer has tokens in pool |
|  |   |   |   |   |
| App Backend  | approve | approve a pool | pool must comply with requirements | pool is enabled  |
| App Backend  | disable | disable a pool | pool must exist | pool is disabled  |
| App Backend  | whitelist | add account to whitelist | account must exist and comply with requirements | user appears on whitelist 
| App Backend | blacklist | add account to blacklist | account must exist | user removed from whitelist   |

## Smart Contract Accounts 

The following eosio acconts have been created for smart contracts

### EOS Mainnet

#### [tokensaleapp](https://bloks.io/account/tokensaleapp)
#### [tokenpoolapp](https://bloks.io/account/tokensaleapp)

### Jungle3 testnet 

#### [tokensaleapp](https://jungle3.bloks.io/account/tokensaleapp)
#### [tokenpoolapp](https://jungle3.bloks.io/bloks.io/account/tokensaleapp)

# Contributing

Please Read EOS Costa Rica's [Open Source Contributing Guidelines](https://developers.eoscostarica.io/docs/open-source-guidelines).

Please report bugs big and small by [opening an issue](https://github.com/eostarter/eostarter.sale-smart-contract/issues)

# About EOSstarter

<p align="center">
	<a href="https://eostarter.org">
		<img src="./docs/img/logo-words.png" width="300">
	</a>
</p>
<br/>

EOStarter is a Community-powered Incubator and Gamified Launchpad for EOS projects.

[eostarter.org](https://eostarter.org/)