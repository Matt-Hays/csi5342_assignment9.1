###  Installation & First Run  (Ubuntu 22.04/24.04)

These instructions assume a vanilla Ubuntu system.  
For other operating systems, please consult each tool’s official documentation.

```bash
# 1 – Fetch the source
git clone https://github.com/your-org/microinfusion_pump_controller.git
cd microinfusion_pump_controller
sudo apt update

# 2 – Toolchain for Ceedling (unit-testing framework)
sudo apt install ruby build-essential # Ruby + compiler toolchain
gem install ceedling # install Ceedling gem

# 3 – Auxiliary CLI tools
sudo apt install bear # build-trace generator (generates compile_commands.json)
sudo apt install gcovr # coverage report generator

# 4 – Dependencies for Dextool Mutate (mutation testing)
sudo apt install -y build-essential git cmake \
                    llvm-18 llvm-18-dev clang-18 libclang-18-dev \
                    sqlite3 libsqlite3-dev

# 5 – D language tool-chain (used by Dextool)
sudo apt install ldc dub # LLVM-based D compiler + Dub

# 6 – Build Dextool (≥8 GB RAM recommended — see note below)
dub fetch dextool
# Optional low-RAM build:
#   dub build -c lowmem --force
dub run dextool -- -h # compiles once, shows help

# 7 – Mutation-testing workflow  (edit .dextool_mutate.toml first!)
bear -- ceedling clobber test:all # fresh build + compile DB
dextool mutate admin --init # one-time project scaffold
dextool mutate analyze # discover mutation points
dextool mutate test # build mutants & run tests
dextool mutate report # generate HTML dashboard

# 8 – Line-by-line coverage report
gcovr --root . --html-details -o coverage.html
```

**Low-memory tip**  
If the first `dub run dextool` fails with an **out-of-memory (OOM)** error, rebuild once with  
```bash
dub build -c lowmem --force
```  
…then re-run:  
```bash
dub run dextool -- -h
```
