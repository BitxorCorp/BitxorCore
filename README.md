# Bitxorcore


Bitxor-based networks rely on nodes to provide a trustless, high-performance, and secure blockchain platform.

These nodes are deployed using [bitxorcore] software, a C++ Blockchain Core

Learn more about the protocol by reading the [whitepaper](https://docs.bitxor.org/whitepaper-en.pdf) and the [developer documentation](https://docs.bitxor.org).

## Package Organization

BitxorCore code is organized as follows:

| Folder name | Description |
| -------------|--------------|
| /extensions | Modules that add features to the bare bitxorcore. These capabilities are all optional because none of them impact consensus. |
| /external | External dependencies that are built with the client. |
| /plugins | Modules that introduce new and different ways to alter the chain's state via transactions. |
| /resources | Default properties. These can be configured per network and node. |
| /scripts | Utility scripts for developers. |
| /sdk | Reusable code used by tests and tools. |
| /seed | Genesis blocks used in tests. |
| /src | BitxorCore engine. |
| /tests | Collection of tests. |
| /tools | Tools to deploy and monitor networks and nodes. |

## Versioning

Versioning starting with 1.1.0.1 release. Testnet releases use odd numbers for minor field.

| description | versions |
|-------------|----------|
| example mainnet build numbers | 1.0.0.0, 1.2.x.x, 1.4.x.x, 2.2.x.x, 3.4.x.x |
| example testnet build numbers | 1.1.0.0, 1.3.x.x, 1.5.x.x, 2.3.x.x, 3.5.x.x |

## Building the Image

To compile bitxorcore source code, follow the [developer notes](docs/README.md).

## Running bitxorcore-client

bitxorcore executable can be used either to run different types of nodes or to launch new networks. This section contains the instructions on how to run the bitxorcore for various purposes.

### Test Network Node

Developers can deploy testnet nodes to experiment with the offered transaction set in a live network without the loss of valuable assets.

To run a test net node, follow [this guide](https://docs.bitxor.org/guides/network/running-a-testnet-node.html).

### Main Network Node

At the time of writing, the main public network has not been launched. Follow the latest updates about the public launch on the [Forum](https://forum.bitxor.org).

### Private Test Network

With Bitxor, businesses can launch and extend private networks by developing custom plugins and extensions at the protocol level. The package [Service Bootstrap] contains the necessary setup scripts to deploy a network for testing and development purposes with just one command.

To run a private testnet, follow [this guide](https://docs.bitxor.org/guides/network/creating-a-private-testnet.html).

## Contributing

Before contributing please [read this](CONTRIBUTING.md).

## Getting Help

- [Bitxor Developer Documentation](https://docs.bitxor.org)
- Join the community [slack group (#help)][slack]
- If you found a bug, [open a new issue][issues]

## License

Copyright (c) 2022 Kriptxor Corp, Microsula S.A., Licensed under the [GNU Lesser General Public License v3](LICENSE.txt)

[developer documentation]: https://docs.bitxor.org
[Forum]: https://forum.bitxor.org
[issues]: https://github.com/bitxorcorp/bitxorcore/issues
[bitxorcore]: https://github.com/bitxorcorp/bitxorcore
[bitxor-apirest]: https://github.com/bitxor/bitxor-apirest
[bitxor-bootstrap]: https://github.com/bitxor/bitxor-bootstrap
[bitxor]: https://www.bitxor.org
[whitepaper]: https://docs.bitxor.org/whitepaper-en.pdf
