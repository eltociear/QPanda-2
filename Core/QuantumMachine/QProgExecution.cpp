#include "Core/QuantumMachine/QProgExecution.h"
#include "Core/Utilities/QProgInfo/QProgProgress.h"
#include <float.h>
#include <random>

USING_QPANDA
using namespace std;

static bool compareQubit(Qubit * a, Qubit * b)
{
    return a->getPhysicalQubitPtr()->getQubitAddr() <
        b->getPhysicalQubitPtr()->getQubitAddr();
}

static bool Qubitequal(Qubit * a, Qubit * b)
{
    return a->getPhysicalQubitPtr()->getQubitAddr() == 
        b->getPhysicalQubitPtr()->getQubitAddr();
}

static void _check_repetitive_qubit(const QVec& target_qv, const QVec& ctrl_qv)
{
	auto total_size = target_qv.size() + ctrl_qv.size();
	auto qv = target_qv + ctrl_qv;
	if (total_size != qv.size())
	{
		QCERR("target_qv == ctrl_qv ");
		throw invalid_argument("target_qv == ctrl_qv");
	}

	for (int i = 0; i < ctrl_qv.size(); i++)
	{
		for (int j = i + 1; j < ctrl_qv.size(); j++)
		{
			if (qv[i]->get_phy_addr() == qv[j]->get_phy_addr())
			{
				QCERR("gate have the same qubit ");
				throw invalid_argument("gate have the same qubit ");
			}
		}
	}
}


void QProgExecution::execute(std::shared_ptr<AbstractQGateNode> cur_node,
    std::shared_ptr<QNode> parent_node,
    TraversalConfig & param,
	QPUImpl*& qpu)
{
    QPANDA_OP(GateType::BARRIER_GATE == cur_node->getQGate()->getGateType(), return);

	bool dagger = cur_node->isDagger() ^ param.m_is_dagger;
    if (cur_node->getTargetQubitNum() <= 0)
    {
        QCERR("Unknown internal error");
        throw runtime_error("Unknown internal error");
    }

    QVec control_qubit_vector;
    for (auto aiter : param.m_control_qubit_vector)
    {
        control_qubit_vector.push_back(aiter);
    }

	cur_node->getControlVector(control_qubit_vector);

    if (control_qubit_vector.size() > 0)
    {
        sort(control_qubit_vector.begin(), 
            control_qubit_vector.end(),
            compareQubit);

        control_qubit_vector.erase(unique(control_qubit_vector.begin(),
                                         control_qubit_vector.end(), Qubitequal),
                                  control_qubit_vector.end());
    }

	QVec target_qubit;
	cur_node->getQuBitVector(target_qubit);

    _check_repetitive_qubit(target_qubit, control_qubit_vector);

    auto qgate = cur_node->getQGate();
    auto aiter = QGateParseMap::getFunction(qgate->getOperationNum());

    if (nullptr == aiter)
    {
        stringstream error;
        error << "gate operation num error ";
        QCERR(error.str());
        throw run_fail(error.str());
    }

    QuantumGate *new_quantum_gate = nullptr;
    if (param.m_rotation_angle_error > DBL_EPSILON ||
        param.m_rotation_angle_error < -DBL_EPSILON)
    {
        qgate_set_rotation_angle_error(cur_node, &new_quantum_gate, param.m_rotation_angle_error);
    }

    if (nullptr == new_quantum_gate)
    {
        aiter(qgate, target_qubit, qpu, dagger, control_qubit_vector, (GateType)qgate->getGateType());
    }
    else
    {
        aiter(new_quantum_gate, target_qubit, qpu, dagger, control_qubit_vector, (GateType)qgate->getGateType());
        delete new_quantum_gate;
    }
    
    /* update qprog exec progress, use QProgExecution address as process id */
    uint64_t exec_id = (uint64_t)this;
    QProgProgress::getInstance().update_processed_gate_num(exec_id);
}


void QProgExecution::execute(std::shared_ptr<AbstractQuantumMeasure> cur_node,
	std::shared_ptr<QNode> parent_node,
	TraversalConfig & param,
	QPUImpl*& qpu)
{
    if (!param.m_can_optimize_measure)
    {
        int iResult = qpu->qubitMeasure(cur_node->getQuBit()->getPhysicalQubitPtr()->getQubitAddr());
        if (iResult < 0)
        {
            QCERR("result error");
            throw runtime_error("result error");
        }
        CBit * cexpr = cur_node->getCBit();
        if (nullptr == cexpr)
        {
            QCERR("unknow error");
            throw runtime_error("unknow error");
        }

        cexpr->set_val(iResult);
        string name = cexpr->getName();
        auto aiter = m_result.find(name);
        if (aiter != m_result.end())
        {
            aiter->second = (bool)iResult;
        }
        else
        {
            m_result.insert(pair<string, bool>(name, (bool)iResult));
        }
    }
}

void QProgExecution::execute(std::shared_ptr<AbstractQuantumReset> cur_node,
	std::shared_ptr<QNode> parent_node,
	TraversalConfig & param,
	QPUImpl*& qpu)
{
    qpu->Reset(cur_node->getQuBit()->getPhysicalQubitPtr()->getQubitAddr());
}

void QProgExecution::execute(std::shared_ptr<AbstractControlFlowNode> cur_node, std::shared_ptr<QNode> parent_node, TraversalConfig &param, QPUImpl *&qpu)
{
    if (nullptr == cur_node)
    {
        QCERR("control_flow_node is nullptr");
        throw std::invalid_argument("control_flow_node is nullptr");
    }

    auto node = std::dynamic_pointer_cast<QNode>(cur_node);
    if (nullptr == node)
    {
        QCERR("Unknown internal error");
        throw std::runtime_error("Unknown internal error");
    }
    auto node_type = node->getNodeType();

    auto cexpr = cur_node->getCExpr();
    if (WHILE_START_NODE == node_type)
    {
        while(cexpr.get_val())
        {
            auto true_branch_node = cur_node->getTrueBranch();
            Traversal::traversalByType(true_branch_node, node, *this,param,qpu);
        }
    }
    else if (QIF_START_NODE == node_type)
    {
        if (cexpr.get_val())
        {
            auto true_branch_node = cur_node->getTrueBranch();
            Traversal::traversalByType(true_branch_node, node, *this,param,qpu);
        }
        else
        {
            auto false_branch_node = cur_node->getFalseBranch();
            if (nullptr != false_branch_node)
            {
                Traversal::traversalByType(false_branch_node, node, *this,param,qpu);
            }
        }
    }

    return ;
}

void QProgExecution::execute(std::shared_ptr<AbstractQuantumCircuit> cur_node,
    std::shared_ptr<QNode> parent_node,
    TraversalConfig & param,
    QPUImpl*& qpu)
{
    bool save_dagger = param.m_is_dagger;
    param.m_is_dagger = cur_node->isDagger() ^ param.m_is_dagger;
    QVec control_qubit_vector;
    cur_node->getControlVector(control_qubit_vector);
    auto size_bak = param.m_control_qubit_vector.size();
    param.m_control_qubit_vector.insert(param.m_control_qubit_vector.end(),
                                        control_qubit_vector.begin(), control_qubit_vector.end());

    if (param.m_is_dagger)
    {
        auto iter = cur_node->getLastNodeIter();
        for (; iter != cur_node->getHeadNodeIter(); --iter)
        {
            auto node = *iter;
            if (nullptr == node)
            {
                QCERR("node is null");
                std::runtime_error("node is null");
            }
            
            Traversal::traversalByType(node,dynamic_pointer_cast<QNode>(cur_node),*this,param,qpu);
        }

    }
    else
    {
        auto iter = cur_node->getFirstNodeIter();
        for (; iter != cur_node->getEndNodeIter(); ++iter)
        {
            auto node = *iter;
            if (nullptr == node)
            {
                QCERR("node is null");
                std::runtime_error("node is null");
            }

            Traversal::traversalByType(node,dynamic_pointer_cast<QNode>(cur_node),*this,param,qpu);
        }
    }

    param.m_is_dagger = save_dagger;
    param.m_control_qubit_vector.erase(param.m_control_qubit_vector.begin() + size_bak,
                                       param.m_control_qubit_vector.end());
    return;
}

void QProgExecution::execute(std::shared_ptr<AbstractQNoiseNode> cur_node,
	std::shared_ptr<QNode> parent_node,
	TraversalConfig & param,
	QPUImpl*& qpu)
{
    QVec qvec = cur_node->get_qvec();
    auto matrix = cur_node->get_ops();
    if (qvec.size() <= 0)
    {
        QCERR("Unknown internal error");
        throw runtime_error("Unknown internal error");
    }

    Qnum qnum;
    for_each(qvec.begin(), qvec.end(), [&](Qubit* qubit){ qnum.push_back(qubit->get_phy_addr()); });

    qpu->process_noise(qnum, matrix);
}

void QProgExecution::execute(std::shared_ptr<AbstractQDebugNode> cur_node,
	std::shared_ptr<QNode> parent_node,
	TraversalConfig & param,
	QPUImpl*& qpu)
{
    qpu->debug(cur_node);
}

QProgExecution::QProgExecution()
{
    m_rng.seed(std::random_device()());
}


void QProgExecution::qgate_set_rotation_angle_error(shared_ptr<AbstractQGateNode> gate,
                                                    QuantumGate **new_quantum_gate,
                                                    double rotation_angle_error)
{
    QuantumGate *quantum_gate = gate->getQGate();
    auto gate_type = static_cast<GateType>(quantum_gate->getGateType());
    auto gate_name = TransformQGateType::getInstance()[gate_type];

    switch (gate_type)
    {
    case GateType::RX_GATE:
    case GateType::RY_GATE:
    case GateType::RZ_GATE:
    case GateType::U1_GATE:
    case GateType::CPHASE_GATE:
    case GateType::ISWAP_THETA_GATE:
    {
        auto angle_param = dynamic_cast<QGATE_SPACE::AbstractSingleAngleParameter *>(quantum_gate);
        auto angle = angle_param->getParameter();
        angle = random_generator19937(angle - rotation_angle_error, angle + rotation_angle_error);
        *new_quantum_gate = QGATE_SPACE::create_quantum_gate(gate_name, angle);
    }
    break;
    case GateType::U2_GATE:
    {
        QGATE_SPACE::U2 *u2_gate = dynamic_cast<QGATE_SPACE::U2*>(quantum_gate);
        auto phi = u2_gate->get_phi();
        auto lambda = u2_gate->get_lambda();

        phi = random_generator(phi - rotation_angle_error, phi + rotation_angle_error);
        lambda = random_generator(lambda - rotation_angle_error, lambda + rotation_angle_error);
        *new_quantum_gate = QGATE_SPACE::create_quantum_gate(gate_name, phi, lambda);
    }
    break;
    case GateType::U3_GATE:
    {
        QGATE_SPACE::U3 *u3_gate = dynamic_cast<QGATE_SPACE::U3*>(quantum_gate);
        auto theta = u3_gate->get_theta();
        auto phi = u3_gate->get_phi();
        auto lambda = u3_gate->get_lambda();

        theta = random_generator(theta - rotation_angle_error, theta + rotation_angle_error);
        phi = random_generator(phi - rotation_angle_error, phi + rotation_angle_error);
        lambda = random_generator(lambda - rotation_angle_error, lambda + rotation_angle_error);
        *new_quantum_gate = QGATE_SPACE::create_quantum_gate(gate_name, theta, phi, lambda);
    }
    break;
    case GateType::U4_GATE:
    {
        auto angle_param = dynamic_cast<QGATE_SPACE::AbstractAngleParameter *>(quantum_gate);
        auto alpha = angle_param->getAlpha();
        auto beta = angle_param->getBeta();
        auto gamma = angle_param->getGamma();
        auto delta = angle_param->getDelta();

        alpha = random_generator(alpha - rotation_angle_error, alpha + rotation_angle_error);
        beta = random_generator(beta - rotation_angle_error, beta + rotation_angle_error);
        gamma = random_generator(gamma - rotation_angle_error, gamma + rotation_angle_error);
        delta = random_generator(delta - rotation_angle_error, delta + rotation_angle_error);
        *new_quantum_gate = QGATE_SPACE::create_quantum_gate(gate_name, alpha, beta, gamma, delta);
    }
    break;
    default:
        *new_quantum_gate = nullptr;
        break;
    }

    return;
}
