#!/usr/bin/env bash

# ==============================================================================
# Strict Mode:
# -e: Exit immediately if a command exits with a non-zero status.
# -u: Treat unset variables as an error.
# -o pipefail: The return value of a pipeline is the status of the last command to exit.
# ==============================================================================
set -euo pipefail

# --- Configuration ---
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEMO_BIN="${PROJECT_ROOT}/examples/bin/Webull-SDK-Demo"

# --- Logging Utilities ---
GREEN=$(tput setaf 2 || echo "")
BLUE=$(tput setaf 4 || echo "")
RED=$(tput setaf 1 || echo "")
YELLOW=$(tput setaf 3 || echo "")
BOLD=$(tput bold || echo "")
RESET=$(tput sgr0 || echo "")

log_info()    { echo -e "${BLUE}${BOLD}[INFO]${RESET} $1"; }
log_success() { echo -e "${GREEN}${BOLD}[SUCCESS]${RESET} $1"; }
log_error()   { echo -e "${RED}${BOLD}[ERROR]${RESET} $1"; >&2; }

# --- Pre-flight Checks ---
log_info "Checking for compiled binary..."

if [[ ! -f "${DEMO_BIN}" ]]; then
    log_error "Demo binary not found!"
    log_error "Expected location: ${DEMO_BIN}"
    log_info "Please compile the project first by running: ./build.sh"
    exit 1
fi

# Ensure the binary has execution permissions
if [[ ! -x "${DEMO_BIN}" ]]; then
    log_info "Adding execution permissions to the binary..."
    chmod +x "${DEMO_BIN}"
fi

# --- Execution ---
log_info "Launching Webull-SDK-Demo..."
log_info "Arguments passed to binary: ${*:-<none>}"

echo ""
echo "${YELLOW}==================== APPLICATION OUTPUT ====================${RESET}"

set +e
"${DEMO_BIN}" "$@"
APP_EXIT_CODE=$?
set -e

echo "${YELLOW}============================================================${RESET}"

# --- Result Evaluation ---
if [[ ${APP_EXIT_CODE} -eq 0 ]]; then
    log_success "Application finished successfully (Exit Code 0)."
fi