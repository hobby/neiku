#/bin/bash

# brief: test for log.sh

source ../../sh/log.sh

log_init $0 `log_level_dbg`
log_verbose_on

function test()
{
    log_err "this is error msg"
    log_msg "this is normal msg"
    log_dbg "this is debug msg"
}
test
