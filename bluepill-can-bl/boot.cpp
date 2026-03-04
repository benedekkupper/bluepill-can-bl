#include "interface/can_bl/boot.hpp"
#include "stm32f1/bkp.hpp"

namespace can_bl
{
request_source pop_update_request()
{
    BKP.enable_access();
    request_source fwup_req = request_source::none;
    if (BKP.DR[0] == uint16_t(~BKP.DR[1]))
    {
        switch (static_cast<request_source>(BKP.DR[0]))
        {
        case request_source::app:
        case request_source::loader:
            fwup_req = static_cast<request_source>(BKP.DR[0]);
            break;
        default:
            break;
        }
    }
    // clear request
    BKP.DR[0] = 0;
    BKP.DR[1] = 0;
    BKP.disable_access();
    return fwup_req;
}

void set_update_request(request_source source)
{
    BKP.enable_access();
    auto key = static_cast<uint16_t>(source);
    BKP.DR[0] = key;
    BKP.DR[1] = uint16_t(~key);
    BKP.disable_access();
}

} // namespace can_bl
