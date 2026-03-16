# Packages

`packages/` contains all firmware implementations in this monorepo.

## Directories

- `internal/`: reference packages, prototypes, and experimental implementations
- `products/`: public-facing device packages that represent real Volttio variants

## Conventions

- Every package owns its own implementation root and can choose the toolchain that fits it best.
- Package slugs use lowercase kebab-case in format `<model-name>-<tier>`.
- Public display names use the format `Volttio <ModelName> <Tier>`.
