#include <assert.h>
#include <iostream>
#include <tinyxml/tinyxml.h>

using namespace std;

int main() {
    TiXmlDocument *m_xml_file = new TiXmlDocument();
    bool rt = m_xml_file->LoadFile("config.xml");
    assert(rt);

    TiXmlElement* root = m_xml_file->RootElement();
    TiXmlElement* log_node = root->FirstChildElement("log");
    TiXmlElement* node = log_node->FirstChildElement("log_path");
    if(!node || !node->GetText()) {
        exit(0);
    }
    std::string log_path = node->GetText();

    node = log_node->FirstChildElement("log_max_size");
    if(!node || !node->GetText()) {
        exit(0);
    }
    int32_t max_size = atoi(node->GetText());

    node = log_node->FirstChildElement("log_level");
    if(!node || !node->GetText()) {
        exit(0);
    }
    std::string log_level = node->GetText();


    node = log_node->FirstChildElement("log_sync_interval");
    if(!node || !node->GetText()) {
        exit(0);
    }
    int32_t log_sync_interval = atoi(node->GetText());

    cout << log_path << '\n' << max_size << '\n' << log_level << '\n' << log_sync_interval << '\n';

    return 0;
}