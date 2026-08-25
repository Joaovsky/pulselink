#include <zephyr/ztest.h>
#include "control_fsm.hpp"

ZTEST_SUITE(fsm, NULL, NULL, NULL, NULL, NULL);

ZTEST(fsm, classify) {
    ControlFsm f;
    zassert_true(f.update(2000, 100) == Health::NORMAL);
    zassert_true(f.update(6500, 100) == Health::WARNING);
    zassert_true(f.update(8500, 100) == Health::FAULT);
}
