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

#include "QuantumMachineFactory.h"
#include "OriginQuantumMachine.h"
#include "Core/QuantumMachine/QCloudMachine.h"
using namespace std;
USING_QPANDA
QMachineStatus * QMachineStatusFactory::GetQMachineStatus()
{
    return new OriginQMachineStatus();
}

QuantumMachineFactory::QuantumMachineFactory()
{

}

QuantumMachineFactoryHelper::
QuantumMachineFactoryHelper(
    std::string _Classname,
    constructor_t _Constructor
)
{
    // create a static variable of the helper class
    // can execute the constructor code
    
    // Here we insert the classname and constructor pair
    // into the map of _Quantum_Machine_Constructor
    auto &fac=
        QuantumMachineFactory::GetFactoryInstance();
    fac.registerclass(_Classname, _Constructor);
}

QuantumMachineFactory & QuantumMachineFactory::GetFactoryInstance()
{
    static QuantumMachineFactory fac;
    return fac;
}

/* Factory Method */
QuantumMachine * QuantumMachineFactory::
    CreateByName(std::string _Name)
{
    // create a Quantum Machine by its name string
    if (_Quantum_Machine_Constructor.find(_Name)
        == _Quantum_Machine_Constructor.end())
    {
        // that means the quantum machine is not registered
        return nullptr;
    }
    return QuantumMachineFactory::
        _Quantum_Machine_Constructor[_Name]();
}

QuantumMachine* QuantumMachineFactory::
CreateByType(QMachineType type)
{
    QuantumMachine* qm;
    switch (type)
    {
    case QMachineType::CPU:
        qm = new CPUQVM();
        break;
    case QMachineType::CPU_SINGLE_THREAD:
        qm = new CPUSingleThreadQVM();
        break;
    case QMachineType::GPU:
        qm = new GPUQVM();
        break;
    case QMachineType::NOISE:
        qm = new NoiseQVM();
        break;
    case QMachineType::QCloud:
#ifdef USE_CURL
        qm = new QCloudMachine();
#else
        throw init_fail("no libcurl found");
#endif
        break;
    default:
        qm = nullptr;
        break;
    }

    return qm;
}

void QuantumMachineFactory::registerclass(std::string name, constructor_t constructor)
{
    if (_Quantum_Machine_Constructor.find(name) != _Quantum_Machine_Constructor.end())
    {
        cerr << "QuantumMachineFactory Warning:" << endl
             << "re-register the class " << name << endl;
    }
    _Quantum_Machine_Constructor.insert(make_pair(name, constructor));
}











