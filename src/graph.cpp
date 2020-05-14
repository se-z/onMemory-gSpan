#include "graph.h"

namespace GRAPH {

template <class T, class Iterator>
void tokenize(const char *str, Iterator iterator) {
    std::istringstream is(str);
    std::copy(std::istream_iterator<T>(is), std::istream_iterator<T>(),
              iterator);
}

void Graph::buildEdge() {

    char buf[512];
    std::map<std::string, unsigned int> tmp;
    unsigned int id = 0;

    for(int from = 0; from < (int)size(); ++from) {
        for(Vertex::edge_iterator it = (*this)[from].edge.begin();
            it != (*this)[from].edge.end(); ++it) {
            if(directed || from <= it->to)
                std::sprintf(buf, "%d %d %d", from, it->to, it->elabel);
            else
                std::sprintf(buf, "%d %d %d", it->to, from, it->elabel);

            // Assign unique id's for the edges.
            if(tmp.find(buf) == tmp.end()) {
                it->id = id;
                tmp[buf] = id;
                ++id;
            } else {
                it->id = tmp[buf];
            }
        }
    }

    edge_size_ = id;
}

std::istream &Graph::read(std::istream &is) {

    std::vector<std::string> result;
    char line[1024];
    clear(); // graphオブジェクトの専有領域をそのままに,
             // 中身に入ってるものを全て消す

    double depth_ctr = 0.0;

    while(true) {

        unsigned int pos =
            is.tellg(); // ストリームバッファの現在の読み取り位置を取得する
        if(!is.getline(line, 1024)) {
            break;
        }

        result.clear();
        tokenize<std::string>(line, std::back_inserter(result));

        if(result.empty()) {
            // do nothing
        } else if(result[0] == "t") {

            if(!empty()) { // use as delimiter
                is.seekg(pos, std::ios_base::beg);
                // std::cout << "empty" << std::endl;
                break;
            } else {
                mol_id = result[1].c_str();
                std::cout << mol_id << std::endl;
                // std::cout << mol_id << std::cout;
                // mol_id = atof(result[3].c_str());
            }
        } else if(result[0] == "v" && result.size() >= 3) {

            unsigned int id = atoi(result[1].c_str());
            this->resize(id + 1);
            (*this)[id].label = atoi(result[2].c_str());
        } else if(result[0] == "e" && result.size() >= 4) {

            depth_ctr += 1.0;
            int from = atoi(result[1].c_str());
            int to = atoi(result[2].c_str());
            int elabel = atoi(result[3].c_str());

            if((int)size() <= from || (int)size() <= to) {
                std::cerr << "Format Error:  define vertex lists before edges"
                          << std::endl;
                exit(-1);
            }

            (*this)[from].push(from, to, elabel);
            if(!directed)
                (*this)[to].push(to, from, elabel);
        }
    }

    buildEdge(); // edgeに順にナンバリングしていってる?.

    return is;
}

std::ostream &Graph::write(std::ostream &os) {
    char buf[512];
    std::set<std::string> tmp;

    for(int from = 0; from < (int)size(); ++from) {
        os << "v " << from << " " << (*this)[from].label << std::endl;

        for(Vertex::edge_iterator it = (*this)[from].edge.begin();
            it != (*this)[from].edge.end(); ++it) {
            if(directed || from <= it->to)
                std::sprintf(buf, "%d %d %d", from, it->to, it->elabel);
            else
                std::sprintf(buf, "%d %d %d", it->to, from, it->elabel);
            tmp.insert(buf);
        }
    }

    for(std::set<std::string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        os << "e " << *it << std::endl;

    return os;
}

void Graph::check(void) {
    /* Check all indices
     */
    for(int from = 0; from < (int)size(); ++from) {
        for(Vertex::edge_iterator it = (*this)[from].edge.begin();
            it != (*this)[from].edge.end(); ++it) {
            assert(it->from >= 0 && it->from < size());
            assert(it->to >= 0 && it->to < size());
        }
    }
}

void Vertex::push(int from, int to, int elabel) {
    edge.resize(edge.size() + 1);
    edge[edge.size() - 1].from = from;
    edge[edge.size() - 1].to = to;
    edge[edge.size() - 1].elabel = elabel;
    return;
}

Graph::Graph(bool _directed) { directed = _directed; }

void Graph::__registerEdge_size(int edge_size) { edge_size_ = edge_size; }

unsigned int Graph::vertex_size() { return (unsigned int)size(); } // wrapper

} // namespace GRAPH
