#include "datasingleton.h"

DataSingleton::DataSingleton() {}

DataSingleton* DataSingleton::Instance(){
    if(!m_pInstance){
        m_pInstance = new DataSingleton;
    }
    return m_pInstance;
}
