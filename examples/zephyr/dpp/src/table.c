/*$file${src::table.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: dpp.qm
* File:  ${src::table.c}
*
* This code has been generated by QM 5.2.4 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${src::table.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* Active object class -----------------------------------------------------*/
/*$declare${AOs::Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table} ............................................................*/
typedef struct Table {
/* protected: */
    QActive super;

/* public: */

/* private: */
    uint8_t fork[N_PHILO];
    uint8_t isHungry[N_PHILO];
} Table;
extern Table Table_inst;

/* protected: */
static QState Table_initial(Table * const me, void const * const par);
static QState Table_active(Table * const me, QEvt const * const e);
static QState Table_serving(Table * const me, QEvt const * const e);
static QState Table_paused(Table * const me, QEvt const * const e);
/*$enddecl${AOs::Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1U)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1U) % N_PHILO))
#define FREE      ((uint8_t)0)
#define USED      ((uint8_t)1)

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 700U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.0.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::AO_Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_Table} .........................................................*/
QActive * const AO_Table = &Table_inst.super; /* "opaque" pointer to Table AO */
/*$enddef${AOs::AO_Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Table_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table_ctor} .......................................................*/
void Table_ctor(void) {
    Table *me = &Table_inst;
    uint8_t n;

    QActive_ctor(&me->super, Q_STATE_CAST(&Table_initial));

    for (n = 0U; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0U;
    }
}
/*$enddef${AOs::Table_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Table} ............................................................*/
Table Table_inst;

/*${AOs::Table::SM} ........................................................*/
static QState Table_initial(Table * const me, void const * const par) {
    /*${AOs::Table::SM::initial} */
    uint8_t n;
    (void)par; /* unused parameter */

    QS_OBJ_DICTIONARY(&Table_inst);

    QS_SIG_DICTIONARY(DONE_SIG,      (void *)0); /* global signals */
    QS_SIG_DICTIONARY(EAT_SIG,       (void *)0);
    QS_SIG_DICTIONARY(PAUSE_SIG,     (void *)0);
    QS_SIG_DICTIONARY(SERVE_SIG,     (void *)0);
    QS_SIG_DICTIONARY(TEST_SIG,      (void *)0);

    QS_SIG_DICTIONARY(HUNGRY_SIG,    me); /* signal just for Table */

    QActive_subscribe(&me->super, DONE_SIG);
    QActive_subscribe(&me->super, PAUSE_SIG);
    QActive_subscribe(&me->super, SERVE_SIG);
    QActive_subscribe(&me->super, TEST_SIG);

    for (n = 0U; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0U;
        BSP_displayPhilStat(n, "thinking");
    }

    QS_FUN_DICTIONARY(&Table_active);
    QS_FUN_DICTIONARY(&Table_serving);
    QS_FUN_DICTIONARY(&Table_paused);

    return Q_TRAN(&Table_serving);
}

/*${AOs::Table::SM::active} ................................................*/
static QState Table_active(Table * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Table::SM::active::TEST} */
        case TEST_SIG: {
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::EAT} */
        case EAT_SIG: {
            Q_ERROR();
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::Table::SM::active::serving} .......................................*/
static QState Table_serving(Table * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Table::SM::active::serving} */
        case Q_ENTRY_SIG: {
            uint8_t n;
            for (n = 0U; n < N_PHILO; ++n) { /* give permissions to eat... */
                if ((me->isHungry[n] != 0U)
                    && (me->fork[LEFT(n)] == FREE)
                    && (me->fork[n] == FREE))
                {
                    TableEvt *te;

                    me->fork[LEFT(n)] = USED;
                    me->fork[n] = USED;
                    te = Q_NEW(TableEvt, EAT_SIG);
                    te->philoNum = n;
                    QACTIVE_PUBLISH(&te->super, &me->super);
                    me->isHungry[n] = 0U;
                    BSP_displayPhilStat(n, "eating  ");
                }
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::serving::HUNGRY} */
        case HUNGRY_SIG: {
            uint8_t n, m;

            n = Q_EVT_CAST(TableEvt)->philoNum;
            /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));

            BSP_displayPhilStat(n, "hungry  ");
            m = LEFT(n);
            /*${AOs::Table::SM::active::serving::HUNGRY::[bothfree]} */
            if ((me->fork[m] == FREE) && (me->fork[n] == FREE)) {
                TableEvt *pe;
                me->fork[m] = USED;
                me->fork[n] = USED;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = n;
                QACTIVE_PUBLISH(&pe->super, &me->super);
                BSP_displayPhilStat(n, "eating  ");
                status_ = Q_HANDLED();
            }
            /*${AOs::Table::SM::active::serving::HUNGRY::[else]} */
            else {
                me->isHungry[n] = 1U;
                status_ = Q_HANDLED();
            }
            break;
        }
        /*${AOs::Table::SM::active::serving::DONE} */
        case DONE_SIG: {
            uint8_t n, m;
            TableEvt *pe;

            n = Q_EVT_CAST(TableEvt)->philoNum;
            /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));

            BSP_displayPhilStat(n, "thinking");
            m = LEFT(n);
            /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = FREE;
            me->fork[n] = FREE;
            m = RIGHT(n); /* check the right neighbor */

            if ((me->isHungry[m] != 0U) && (me->fork[m] == FREE)) {
                me->fork[n] = USED;
                me->fork[m] = USED;
                me->isHungry[m] = 0U;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QACTIVE_PUBLISH(&pe->super, &me->super);
                BSP_displayPhilStat(m, "eating  ");
            }
            m = LEFT(n); /* check the left neighbor */
            n = LEFT(m); /* left fork of the left neighbor */
            if ((me->isHungry[m] != 0U) && (me->fork[n] == FREE)) {
                me->fork[m] = USED;
                me->fork[n] = USED;
                me->isHungry[m] = 0U;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QACTIVE_PUBLISH(&pe->super, &me->super);
                BSP_displayPhilStat(m, "eating  ");
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::serving::EAT} */
        case EAT_SIG: {
            Q_ERROR();
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::serving::PAUSE} */
        case PAUSE_SIG: {
            status_ = Q_TRAN(&Table_paused);
            break;
        }
        default: {
            status_ = Q_SUPER(&Table_active);
            break;
        }
    }
    return status_;
}

/*${AOs::Table::SM::active::paused} ........................................*/
static QState Table_paused(Table * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Table::SM::active::paused} */
        case Q_ENTRY_SIG: {
            BSP_displayPaused(1U);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::paused} */
        case Q_EXIT_SIG: {
            BSP_displayPaused(0U);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::paused::SERVE} */
        case SERVE_SIG: {
            status_ = Q_TRAN(&Table_serving);
            break;
        }
        /*${AOs::Table::SM::active::paused::HUNGRY} */
        case HUNGRY_SIG: {
            uint8_t n = Q_EVT_CAST(TableEvt)->philoNum;
            /* philo ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));
            me->isHungry[n] = 1U;
            BSP_displayPhilStat(n, "hungry  ");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Table::SM::active::paused::DONE} */
        case DONE_SIG: {
            uint8_t n, m;

            n = Q_EVT_CAST(TableEvt)->philoNum;
            /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (me->isHungry[n] == 0U));

            BSP_displayPhilStat(n, "thinking");
            m = LEFT(n);
            /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = FREE;
            me->fork[n] = FREE;
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&Table_active);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
