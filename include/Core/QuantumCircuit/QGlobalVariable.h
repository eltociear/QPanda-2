/*
Copyright (c) 2017-2020 Origin Quantum Computing. All Right Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
/*! \file QGlobalVariable.h */
#ifndef _QGlobalVariable_H
#define _QGlobalVariable_H
#include "Core/Utilities/QPandaNamespace.h"
#include <complex>
#include <vector>
#include <float.h>

QPANDA_BEGIN

/*
*  @brief   QPanda2 node type
*/
enum NodeType
{
    NODE_UNDEFINED = -1,/**< Undefined node */
    GATE_NODE,/**< Quantum gate node */
    CIRCUIT_NODE,/**< Quantum circuit node */
    PROG_NODE,/**< Quantum program node */
    MEASURE_GATE,/**< Quantum measure node */
    WHILE_START_NODE,/**< Quantum while controlflow start node */
    QIF_START_NODE,/**< Quantum if controlflow start node */
    CLASS_COND_NODE,/**< Quantum classical condition node */
    QWAIT_NODE,     /**< QWait node */
	RESET_NODE,     /**< QReset node */
    NOISE_NODE,     /**< Noise simulate virtual node */
    DEBUG_NODE      /**< Debug qubit state vector node */
};

/*
*  @brief   QPanda2 quantum gate type
*/
enum GateType {
	GATE_NOP = -2,
    GATE_UNDEFINED = -1,
    P0_GATE,/**< Quantum p0 gate */
    P1_GATE,/**< Quantum p1 gate */
    PAULI_X_GATE,/**< Quantum pauli x  gate */
    PAULI_Y_GATE,/**< Quantum pauli y  gate */
    PAULI_Z_GATE,/**< Quantum pauli z  gate */
    X_HALF_PI,/**< Quantum x half gate */
    Y_HALF_PI,/**< Quantum y half gate */
    Z_HALF_PI,/**< Quantum z half gate */
	P_GATE,/**<Quantum p gate>*/
    HADAMARD_GATE,/**< Quantum hadamard gate */
    T_GATE,/**< Quantum t gate */
    S_GATE,/**< Quantum s gate */
    RX_GATE,/**< Quantum rotation x gate */
    RY_GATE,/**< Quantum rotation y gate */
    RZ_GATE,/**< Quantum rotation z gate */
    RPHI_GATE,
    U1_GATE,/**< Quantum u1 gate */
    U2_GATE,/**< Quantum u2 gate */
    U3_GATE,/**< Quantum u3 gate */
    U4_GATE,/**< Quantum u4 gate */
    CU_GATE,/**< Quantum control-u gate */
    CNOT_GATE,/**< Quantum control-not gate */
    CZ_GATE,/**< Quantum control-z gate */
	CP_GATE,/**<Quantum control-p gate>*/
    RYY_GATE,/**<Quantum ryy gate>*/
    RXX_GATE,/**<Quantum rxx gate>*/
    RZZ_GATE,/**<Quantum rzz gate>*/
    RZX_GATE,/**<Quantum rzx gate>*/
    CPHASE_GATE,/**< Quantum control-rotation gate */
    ISWAP_THETA_GATE,/**< Quantum iswap-theta gate */
    ISWAP_GATE,/**< Quantum iswap gate */
    SQISWAP_GATE,/**< Quantum sqiswap gate */
    SWAP_GATE,/**< Quantum swap gate */
    TWO_QUBIT_GATE,/**< Quantum two-qubit gate */
    P00_GATE,
    P11_GATE,
    TOFFOLI_GATE,
	ORACLE_GATE,
	I_GATE,

    ECHO_GATE,  /**< Only for 6 qubit online projects */
    BARRIER_GATE,  /**< Only for 6 qubit online projects */

    NoiseSingle_GATE,
    NoiseDouble_GATE
};

/*
*  @brief   QPanda2 Operator type
*/
enum Operatortype
{
    OP_AND = 1, /**< And operation   */
    OP_OR, /**< Or operation   */
    OP_NOT,/**< Not operation   */
    OP_ADD, /**< Add operation   */
    OP_MINUS, /**< Minus operation   */
    OP_MULTIPLY, /**< Multiply operation   */
    OP_DIVIDE, /**< Divide operation   */
    OP_MODE,/**< Mode operation   */
    OP_EQUAL,/**< Equal operation   */
    OP_LESS,/**< Less operation   */
    OP_MORE,/**< More operation   */
    OP_NO_MORE,/**< No more operation   */
    OP_NO_LESS,/**< No less operation   */
};


/*
*  @brief   QPanda2 Operator type
*/
enum OperatorType
{
    TYPE_OPERATOR_NODE,
    TYPE_CBIT_NODE,
    TYPE_CINT_NODE
};


/*
*  @brief   QPanda2 metadata gate type
*/
enum MetadataGateType {
    METADATA_SINGLE_GATE, /**<  Metadata single gate  */
    METADATA_DOUBLE_GATE  /**<  Metadata double gate  */
};


/**< @brief Circumference ratio   */
const qstate_type PI = 3.141592653589793238462643383279502884;

/**< @brief Square root of two   */
const qstate_type SQRT2 = 1.414213562373095048801688724209698079;
const qstate_type SQ2 = 0.707106781186547524400844362104849039;

QPANDA_END

#endif
