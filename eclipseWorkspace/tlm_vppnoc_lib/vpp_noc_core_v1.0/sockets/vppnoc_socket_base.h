#ifndef VPPNOC_SOCKET_BASE_H
#define VPPNOC_SOCKET_BASE_H

namespace vppnoc {

class vppnoc_socket_base {

    public:
        explicit vppnoc_socket_base(int = 0);

        int get_socket_id() const;
        void set_socket_id(int);
    

    private:
        int m_socket_id;
};


inline
vppnoc_socket_base::vppnoc_socket_base(int socket_id ):
    m_socket_id(socket_id) {
}


inline int
vppnoc_socket_base::get_socket_id() const {
    return (m_socket_id);
}  
    

inline void
vppnoc_socket_base::set_socket_id(int socket_id) {
    m_socket_id = socket_id;
}

}


#endif //VPPNOC_SOCKET_BASE_H