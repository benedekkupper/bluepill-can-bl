#include "interface/can_bl/boot.hpp"
#include "stm32f1/bkp.hpp"

namespace can_bl
{
constexpr uint16_t UPDATE_REQUEST_MAGIC = 0xCAB0;

bool pop_update_request()
{
    BKP_t::enable_access();
    bool fwup_req =
        (BKP.DR[0] == UPDATE_REQUEST_MAGIC) and (BKP.DR[1] == uint16_t(~UPDATE_REQUEST_MAGIC));
    // clear request
    BKP.DR[0] = 0;
    BKP.DR[1] = 0;
    BKP_t::disable_access();
    return fwup_req;
}

void set_update_request()
{
    BKP_t::enable_access();
    BKP.DR[0] = UPDATE_REQUEST_MAGIC;
    BKP.DR[1] = uint16_t(~UPDATE_REQUEST_MAGIC);
    BKP_t::disable_access();
}

} // namespace can_bl
