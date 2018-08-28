#include <boost/python.hpp>
#include "xhal/utils/Exception.h"
#include "xhal/XHALDevice.h"
#include "xhal/utils/CalParamTypes.h"
#include "xhal/utils/PyTypes.h"
#include "xhal/rpc/amc.h"
#include "xhal/rpc/calibration_routines.h"
#include "xhal/rpc/daq_monitor.h"
#include "xhal/rpc/optohybrid.h"
#include "xhal/rpc/utils.h"
#include "xhal/rpc/vfat3.h"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

// Boost Python exception translators

//https://stackoverflow.com/questions/9620268/boost-python-custom-exception-class
PyObject* createExceptionClass(const char* name, PyObject* baseTypeObj = PyExc_Exception)
{
    using std::string;
    namespace bp = boost::python;

    string scopeName = bp::extract<string>(bp::scope().attr("__name__"));
    string qualifiedName0 = scopeName + "." + name;
    char* qualifiedName1 = const_cast<char*>(qualifiedName0.c_str());

    PyObject* typeObj = PyErr_NewException(qualifiedName1, baseTypeObj, 0);
    if(!typeObj) bp::throw_error_already_set();
    bp::scope().attr(name) = bp::handle<>(bp::borrowed(typeObj));
    return typeObj;
}

// https://www.boost.org/doc/libs/1_51_0/libs/python/doc/tutorial/doc/html/python/exception.html
#ifndef PY_EXCEPTION_TRANSLATOR
#define PY_EXCEPTION_TRANSLATOR(TRANSLATOR_NAME,EXCEPTION_NAME,EXCEPTION_OBJ)        \
PyObject* EXCEPTION_OBJ = NULL;                                                      \
inline void TRANSLATOR_NAME(EXCEPTION_NAME const& e)                                 \
{                                                                                    \
  assert(EXCEPTION_OBJ != NULL);                                                     \
  /* Use the Python 'C' API to set up an exception object*/                          \
  PyErr_SetString(EXCEPTION_OBJ, e.msg.c_str());                                     \
}
#endif

PY_EXCEPTION_TRANSLATOR(translate_XHALException,xhal::utils::XHALException, obj_XHALException)
PY_EXCEPTION_TRANSLATOR(translate_XHALXMLParserException,xhal::utils::XHALXMLParserException, obj_XHALXMLParserException)
PY_EXCEPTION_TRANSLATOR(translate_XHALRPCException,xhal::utils::XHALRPCException, obj_XHALRPCException)
PY_EXCEPTION_TRANSLATOR(translate_XHALRPCNotConnectedException,xhal::utils::XHALRPCNotConnectedException, obj_XHALRPCNotConnectedException)

// https://stackoverflow.com/questions/7577410/boost-python-select-between-overloaded-methods
uint32_t (xhal::XHALDevice::*readReg_byname)(std::string regName) = &xhal::XHALDevice::readReg;
uint32_t (xhal::XHALDevice::*readReg_byaddress)(uint32_t address) = &xhal::XHALDevice::readReg;

//AMC
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getOHVFATMaskMultiLink_overloads, getOHVFATMaskMultiLink, 0, 1)

//CalRoutines
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(sbitRateScan_overloads, sbitRateScan, 1, 3)

//DaqMonitor
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getmonTRIGGERmain_overloads, getmonTRIGGERmain, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getmonTRIGGEROHmain_overloads, getmonTRIGGEROHmain, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getmonDAQOHmain_overloads, getmonDAQOHmain, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getmonOHmain_overloads, getmonOHmain, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getmonOHSCAmain_overloads, getmonOHSCAmain, 0, 2)

//VFAT3
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readVFAT3ADC_overloads, readVFAT3ADC, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readVFAT3ADCMultiLink_overloads, readVFAT3ADCMultiLink, 2, 3)

BOOST_PYTHON_MODULE(xhalpy){
  using namespace boost::python;

  obj_XHALException = createExceptionClass("XHALException");
  obj_XHALXMLParserException = createExceptionClass("XHALXMLParserException");
  obj_XHALRPCException = createExceptionClass("XHALRPCException");
  obj_XHALRPCNotConnectedException = createExceptionClass("XHALRPCNotConnectedException");

  register_exception_translator<xhal::utils::XHALException>(&translate_XHALException);
  register_exception_translator<xhal::utils::XHALXMLParserException>(&translate_XHALXMLParserException);
  register_exception_translator<xhal::utils::XHALRPCException>(&translate_XHALRPCException);
  register_exception_translator<xhal::utils::XHALRPCNotConnectedException>(&translate_XHALRPCNotConnectedException);

  /*class_<xhal::XHALInterface>("XHALInterface", init<const std::string&>())
    .def("connect",&xhal::XHALInterface::connect)
    .def("loadModule",&xhal::XHALInterface::loadModule)
    .def("setLogLevel",&xhal::XHALInterface::setLogLevel);*/

  class_<xhal::XHALDevice, bases<xhal::XHALInterface> >("XHALDevice", init<const std::string&, const std::string&>())
  class_<xhal::XHALDevice>("XHALDevice", init<const std::string&, const std::string&>())
    .def("connect",&xhal::XHALDevice::connect)
    .def("disconnect",&xhal::XHALDevice::disconnect)
    .def("getBlock",&xhal::XHALDevice::getBlock)
    .def("getList",&xhal::XHALDevice::getList)
    .def("loadModule",&xhal::XHALDevice::loadModule)
    .def("readReg",readReg_byname)
    .def("readReg",readReg_byaddress)
    .def("reconnect",&xhal::XHALDevice::reconnect)
    .def("setLogLevel",&xhal::XHALDevice::setLogLevel)
    .def("writeReg",&xhal::XHALDevice::writeReg);

  class_<PyListUint32>("PyListUint32")
    .def(vector_indexing_suite<PyListUint32>() );

  class_<PyDictUint32<std::string> >("PyDictUint32")
    .def(map_indexing_suite<PyDictUint32<std::string> >() );

  class_<PyDictUint32<int> >("PyDictUint32")
    .def(map_indexing_suite<PyDictUint32<int> >() );

  class_<PyDictVecUint32<std::string> >("PyDictVecUint32")
    .def(map_indexing_suite<PyDictVecUint32<std::string> >() );

  class_<PyDictVecUint32<int> >("PyDictVecUint32")
    .def(map_indexing_suite<PyDictVecUint32<int> >() );

  class_<NestedPyDict<int,PyDictUint32<std::string> > >("NestedPyDict")
    .def(map_indexing_suite<NestedPyDict<int,PyDictUint32<std::string> > >() );

  class_<xhal::rpc::AMC, bases<xhal::XHALDevice> >("AMC", init<const std::string&, const std::string&>())
    .def("getOHVFATMask",&xhal::rpc::AMC::getOHVFATMask)
    .def("getOHVFATMaskMultiLink",&xhal::rpc::AMC::getOHVFATMaskMultiLink,getOHVFATMaskMultiLink_overloads())
    .def("sbitReadOut",&xhal::rpc::AMC::sbitReadOut);

  //class_<xhal::rpc::CalRoutines, bases<xhal::XHALInterface> >("CalRoutines", init<const std::string&>())
  class_<xhal::rpc::CalRoutines>("CalRoutines", init<const std::string&>())
    .def("connect",&xhal::rpc::CalRoutines::connect)
    .def("loadModule",&xhal::rpc::CalRoutines::loadModule)
    .def("setLogLevel",&xhal::rpc::CalRoutines::setLogLevel)
    .def("checkSbitMappingWithCalPulse",&xhal::rpc::CalRoutines::checkSbitMappingWithCalPulse)
    .def("checkSbitRateWithCalPulse",&xhal::rpc::CalRoutines::checkSbitRateWithCalPulse)
    .def("genScan",&xhal::rpc::CalRoutines::genScan)
    .def("genChannelScan",&xhal::rpc::CalRoutines::genChannelScan)
    .def("sbitRateScan",&xhal::rpc::CalRoutines::sbitRateScan,sbitRateScan_overloads())
    .def("ttcGenConf",&xhal::rpc::CalRoutines::ttcGenConf)
    .def("ttcGenToggle",&xhal::rpc::CalRoutines::ttcGenToggle);

  //class_<xhal::rpc::DaqMonitor, bases<xhal::XHALInterface> >("DaqMonitor", init<const std::string&>())
  class_<xhal::rpc::DaqMonitor>("DaqMonitor", init<const std::string&>())
    .def("connect",&xhal::rpc::DaqMonitor::connect)
    .def("loadModule",&xhal::rpc::DaqMonitor::loadModule)
    .def("setLogLevel",&xhal::rpc::DaqMonitor::setLogLevel)
    .def("getmonTTCmain",&xhal::rpc::DaqMonitor::getmonTTCmain)
    .def("getmonTRIGGERmain",&xhal::rpc::DaqMonitor::getmonTRIGGERmain,getmonTRIGGERmain_overloads())
    .def("getmonTRIGGEROHmain",&xhal::rpc::DaqMonitor::getmonTRIGGEROHmain,getmonTRIGGEROHmain_overloads())
    .def("getmonDAQOHmain",&xhal::rpc::DaqMonitor::getmonDAQOHmain,getmonDAQOHmain_overloads())
    .def("getmonOHmain",&xhal::rpc::DaqMonitor::getmonOHmain,getmonOHmain_overloads())
    .def("getmonDAQmain",&xhal::rpc::DaqMonitor::getmonDAQmain)
    .def("getmonOHSCAmain",&xhal::rpc::DaqMonitor::getmonOHSCAmain,getmonOHSCAmain_overloads());

  class_<xhal::ParamCalPulse>("ParamCalPulse")
    .def_readwrite("enable", &xhal::ParamCalPulse::enable)
    .def_readwrite("isCurrent", &xhal::ParamCalPulse::isCurrent)
    .def_readwrite("duration", &xhal::ParamCalPulse::duration)
    .def_readwrite("extVoltStep", &xhal::ParamCalPulse::extVoltStep)
    .def_readwrite("height", &xhal::ParamCalPulse::height)
    .def_readwrite("phase", &xhal::ParamCalPulse::phase)
    .def_readwrite("polarity", &xhal::ParamCalPulse::polarity)
    .def_readwrite("scaleFactor", &xhal::ParamCalPulse::scaleFactor);

  class_<xhal::ParamScan>("ParamScan")
    .def_readwrite("ohN", &xhal::ParamScan::ohN)
    .def_readwrite("ohMask", &xhal::ParamScan::ohMask)
    .def_readwrite("vfatN", &xhal::ParamScan::vfatN)
    .def_readwrite("vfatMask", &xhal::ParamScan::vfatMask)
    .def_readwrite("chan", &xhal::ParamScan::chan)
    .def_readwrite("useUltra", &xhal::ParamScan::useUltra)
    .def_readwrite("useExtTrig", &xhal::ParamScan::useExtTrig)
    .def_readwrite("dacMax", &xhal::ParamScan::dacMax)
    .def_readwrite("dacMin", &xhal::ParamScan::dacMin)
    .def_readwrite("dacSelect", &xhal::ParamScan::dacSelect)
    .def_readwrite("dacStep", &xhal::ParamScan::dacStep)
    .def_readwrite("nevts", &xhal::ParamScan::nevts)
    .def_readwrite("waitTime", &xhal::ParamScan::waitTime)
    .def_readwrite("scanReg", &xhal::ParamScan::scanReg);

  class_<xhal::ParamTtcGen>("ParamTtcGen")
    .def_readwrite("enable", &xhal::ParamTtcGen::enable)
    .def_readwrite("L1Ainterval", &xhal::ParamTtcGen::L1Ainterval)
    .def_readwrite("mode", &xhal::ParamTtcGen::mode)
    .def_readwrite("nPulses", &xhal::ParamTtcGen::nPulses)
    .def_readwrite("pulseDelay", &xhal::ParamTtcGen::pulseDelay)
    .def_readwrite("pulseRate", &xhal::ParamTtcGen::pulseRate)
    .def_readwrite("type", &xhal::ParamTtcGen::type);

  class_<xhal::rpc::Optohybrid, bases<xhal::XHALDevice> >("Optohybrid", init<const std::string&, const std::string&>())
    .def("broadcastRead",&xhal::rpc::Optohybrid::broadcastRead)
    .def("broadcastWrite",&xhal::rpc::Optohybrid::broadcastWrite)
    .def("configureScanModule",&xhal::rpc::Optohybrid::configureScanModule)
    .def("printScanConfiguration",&xhal::rpc::Optohybrid::printScanConfiguration)
    .def("startScanModule",&xhal::rpc::Optohybrid::startScanModule)
    .def("getUltraScanResults",&xhal::rpc::Optohybrid::getUltraScanResults)
    .def("stopCalPulse2AllChannels",&xhal::rpc::Optohybrid::stopCalPulse2AllChannels);

  //class_<xhal::rpc::Utils, bases<xhal::XHALInterface> >("Utils", init<const std::string&>())
  class_<xhal::rpc::Utils>("Utils", init<const std::string&>())
    .def("connect",&xhal::rpc::Utils::connect)
    .def("loadModule",&xhal::rpc::Utils::loadModule)
    .def("setLogLevel",&xhal::rpc::Utils::setLogLevel)
    .def("update_atdb",&xhal::rpc::Utils::update_atdb)
    .def("getRegInfoDB",&xhal::rpc::Utils::getRegInfoDB);

  class_<xhal::rpc::VFAT3, bases<xhal::XHALDevice> >("VFAT3", init<const std::string&, const std::string&>())
    .def("configureVFAT3s",&xhal::rpc::VFAT3::configureVFAT3s)
    .def("configureVFAT3DacMonitor",&xhal::rpc::VFAT3::configureVFAT3DacMonitor)
    .def("configureVFAT3DacMonitorMultiLink",&xhal::rpc::VFAT3::configureVFAT3DacMonitorMultiLink)
    .def("getChannelRegistersVFAT3",&xhal::rpc::VFAT3::getChannelRegistersVFAT3)
    .def("readVFAT3ADC",&xhal::rpc::VFAT3::readVFAT3ADC,readVFAT3ADC_overloads())
    .def("readVFAT3ADCMultiLink",&xhal::rpc::VFAT3::readVFAT3ADCMultiLink,readVFAT3ADCMultiLink_overloads())
    .def("setChannelRegistersVFAT3",&xhal::rpc::VFAT3::setChannelRegistersVFAT3)
    .def("setChannelRegistersVFAT3Simple",&xhal::rpc::VFAT3::setChannelRegistersVFAT3Simple);
}

// Converts a C++ map to a python dict
template <class K, class V>
boost::python::dict convert2dict(const std::map<K,V> inputMap){
    typename std::map<K,V>::const_iterator iter;
    boost::python::dict ret_dict;
    for(iter = inputMap.begin(); iter != inputMap.end(); ++iter){
        ret_dict[(*iter).first] = (*iter).second;
    }

    return ret_dict;
}
