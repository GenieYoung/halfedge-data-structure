#ifndef __halfedge_h__
#define __halfedge_h__

#include <array>
#include <vector>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

namespace halfedge{

/**
 * @brief base struct for element handle types
 */
class ElemHandle
{
    public:
        /* constructor */
        explicit ElemHandle(int idx = -1) : _idx(idx)
        {
        }

        /* get the index of this handle */
        int idx() const
        {
            return _idx;
        }

        /* check whether the handle is valid */
        bool is_valid() const
        {
            return _idx >= 0;
        }

        /* make the handle invalid */
        void invalidate()
        {
            _idx = -1;
        }

        /* comparator */
        bool operator==(const ElemHandle& other) const
        {
            return _idx == other._idx;
        }

        /* comparator */
        bool operator!=(const ElemHandle& other) const
        {
            return _idx != other._idx;
        }

        /* comparator, used for sort */
        bool operator<(const ElemHandle& other) const
        {
            return _idx < other._idx;
        }

        /* increase the index */
        void forward()
        {
            ++_idx;
        }

        /* decrease the index */
        void backward()
        {
            --_idx;
        }

    private:
        /* the handle index */
        int _idx;
};

/* handle for a vertex */
class VertexHandle : public ElemHandle
{
    public:
        explicit VertexHandle(int idx = -1) : ElemHandle(idx)
        {
        }
};

/* handle for a half edge */
class HalfedgeHandle : public ElemHandle
{
    public:
        explicit HalfedgeHandle(int idx = -1) : ElemHandle(idx)
        {
        }
};

/* handle for an edge */
class EdgeHandle : public ElemHandle
{
    public:
        explicit EdgeHandle(int idx = -1) : ElemHandle(idx)
        {
        }
};

/* handle for a face */
class FaceHandle : public ElemHandle
{
    public:
        explicit FaceHandle(int idx = -1) : ElemHandle(idx)
        {
        }
};

/**
 * @brief vertex item
 * @param _halfedge_handle an outgoing halfedge
 */
class VertexItem 
{
    friend class GraphTopology;
    HalfedgeHandle _halfedge_handle;
};

/**
 * @brief half edge item
 * @param _face_handle face contain this halfedge
 * @param _vertex_handle target vertex
 * @param _prev_halfedge_handle previous halfedge
 * @param _next_halfedge_handle next halfedge
 */
class HalfedgeItem
{
    friend class GraphTopology;
    FaceHandle _face_handle;
    VertexHandle _vertex_handle;
    HalfedgeHandle _prev_halfedge_handle;
    HalfedgeHandle _next_halfedge_handle;
};

/**
 * @brief edge item
 * @param _halfedges two side halfedges
 */
class EdgeItem
{
    friend class GraphTopology;
    HalfedgeItem _halfedges[2];
};

/**
 * @brief face item
 * @param _halfedge_handle a halfedge in this face
 */
class FaceItem
{
    friend class GraphTopology;
    HalfedgeHandle _halfedge_handle;
};


class GraphTopology;

/* elem handle connect to the topology graph, allow access to the graph */
class GraphElemHandle
{
    public:
        explicit GraphElemHandle(const GraphTopology* graph) : _graph(graph)
        {
        }

        const GraphTopology* graph() const
        {
            return _graph;
        }

    private:
        const GraphTopology* _graph;
};

class GraphVertexHandle;
class GraphHalfedgeHandle;
class GraphEdgeHandle;
class GraphFaceHandle;

/* vertex handle connect to a topology graph */
class GraphVertexHandle : public GraphElemHandle, public VertexHandle
{
    public:
        explicit GraphVertexHandle(int idx = -1, const GraphTopology* graph = nullptr)
            : GraphElemHandle(graph), VertexHandle(idx)
        {
        }

    public:
        GraphHalfedgeHandle halfedge() const;

        /* check whether the vertex is on boundary*/
        bool is_boundary() const;
};

/* edge handle connect to a topology graph */
class GraphEdgeHandle : public GraphElemHandle, public EdgeHandle
{
    public:
        explicit GraphEdgeHandle(int idx = -1, const GraphTopology* graph = nullptr)
            : GraphElemHandle(graph), EdgeHandle(idx)
        {
        }

    public:
        /* the halfedges near the edge*/
        GraphHalfedgeHandle halfedge(unsigned i) const;

        /* check whether the edge is on boundary*/
        bool is_boundary() const;
};

/* half edge handle connect to a topology graph */
class GraphHalfedgeHandle : public GraphElemHandle, public HalfedgeHandle
{
    public:
        explicit GraphHalfedgeHandle(int idx = -1, const GraphTopology* graph = nullptr)
            : GraphElemHandle(graph), HalfedgeHandle(idx)
        {
        }
    
    public:
        /* next halfedge handle */
        GraphHalfedgeHandle next() const;

        /* prev halfedge handle */
        GraphHalfedgeHandle prev() const;

        /* opposite halfedge handle */
        GraphHalfedgeHandle opposite() const;

        /* source vertex */
        GraphVertexHandle from_vertex() const;

        /* destinate vertex */
        GraphVertexHandle to_vertex() const;

        /* incident edge */
        GraphEdgeHandle edge() const;

        /* incident face */
        GraphFaceHandle face() const;

        /* check whether the halfedge is on boundary*/
        bool is_boundary() const;
};

/* face handle connect to a topology graph */
class GraphFaceHandle : public GraphElemHandle, public FaceHandle
{
    public:
        explicit GraphFaceHandle(int idx = -1, const GraphTopology* graph = nullptr)
            : GraphElemHandle(graph), FaceHandle(idx)
        {
        }
};

template <class Topo, class ElemHandle, class GraphElemHandle>
class IteratorBase
{
    public:
        /* default constructor */
        IteratorBase() : _geh(GraphElemHandle(-1, nullptr))
        {
        }

        /* constructor with mesh and a element handle */
        IteratorBase(const Topo* topo, ElemHandle eh) : _geh(GraphElemHandle(eh.idx(), topo))
        {
        }

        /* dereferencing opeartor */
        const GraphElemHandle& operator*() const
        {
            return _geh;
        }

        /* pointer operator */
        const GraphElemHandle* operator->() const
        {
            return &_geh;
        }

        /* are two iterator equal? */
        bool operator==(const IteratorBase& other) const
        {
            return ((_geh.graph() == other._geh.graph()) && (_geh == other._geh));
        }

        /* are two iterator not equal? */
        bool operator!=(const IteratorBase& other) const
        {
            return !operator==(other);
        }

        /* pre-increment operator */
        IteratorBase& operator++()
        {
            _geh.forward();
            return *this;
        }

        /* post-increment operator */
        IteratorBase operator++(int)
        {
            IteratorBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement operator */
        IteratorBase& operator--()
        {
            _geh.backward();
            return *this;
        }

        /* post-decrement operator */
        IteratorBase operator--(int)
        {
            IteratorBase copy(*this);
            --(*this);
            return copy;
        }

    private:
        GraphElemHandle _geh;
};

template<typename Topo, bool CCW>
class VertexVertexIterBase
{
    public:
        /* default constructor */
        VertexVertexIterBase() : _topo(0), _cycle_count(0)
        {
        }

        VertexVertexIterBase(const Topo* topo, VertexHandle vh, bool end = false)
                         : _topo(topo), _start(_topo->halfedge_handle(vh)), _heh(_start), _cycle_count(static_cast<int>(end))
        {
            // if(CCW)
            // {
            //     int cc = _cycle_count;
            //     ++(*this);
            //     _start = _heh;
            //     _cycle_count = cc;
            // }

            // used for isolate vertex
            if(!_start.is_valid())
                _cycle_count = 0;
        }

    public:
        /* pre-increment */
        VertexVertexIterBase& operator++()
        {
            if(CCW)
            {
                _heh = _topo->ccw_rotated_halfedge_handle(_heh);
                if(_heh == _start)  
                    ++_cycle_count;
            }
            else
            {
                _heh = _topo->cw_rotated_halfedge_handle(_heh);
                if(_heh == _start)
                    ++_cycle_count;
            }
            return *this;
        }

        /* post-increment */
        VertexVertexIterBase operator++(int)
        {
            VertexVertexIterBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement */
        VertexVertexIterBase& operator--()
        {
            if(CCW)
            {
                if(_heh == _start)  
                    --_cycle_count;
                _heh = _topo->cw_rotated_halfedge_handle(_heh);
            }
            else
            {
                if(_heh == _start)
                    --_cycle_count;
                _heh = _topo->ccw_rotated_halfedge_handle(_heh);
            }
            return *this;
        }

        /* post-decrement */
        VertexVertexIterBase operator--(int)
        {
            VertexVertexIterBase copy(*this);
            --(*this);
            return copy;
        }

        /* dereferencing opeartor */
        GraphVertexHandle operator*() const
        {
            return GraphVertexHandle(_topo->to_vertex_handle(_heh).idx(), _topo);
        }

        /* pointer operator */
        GraphVertexHandle* operator->() const
        {
            _geh =  **this;
            return &_geh;
        }

        /* check whether the iterator is valid in the first circulate */
        bool is_valid() const
        {
            return _heh.is_valid() && _cycle_count == 0;
        }

        bool operator==(const VertexVertexIterBase& other) const
        {
            return _topo == other._topo && _start == other._start && _heh == other._heh && _cycle_count == other._cycle_count;
        }

        bool operator!=(const VertexVertexIterBase& other) const
        {
            return !operator==(other);
        }


    public:
        const Topo* _topo;
        HalfedgeHandle _start, _heh;
        int _cycle_count;
        mutable GraphVertexHandle _geh;
};

template<typename Topo, bool CCW>
class VertexOHalfedgeIterBase
{
    public:
        /* default constructor */
        VertexOHalfedgeIterBase() : _topo(0), _cycle_count(0)
        {
        }

        VertexOHalfedgeIterBase(const Topo* topo, VertexHandle vh, bool end = false)
                         : _topo(topo), _start(_topo->halfedge_handle(vh)), _heh(_start), _cycle_count(static_cast<int>(end))
        {
            // if(CCW)
            // {
            //     int cc = _cycle_count;
            //     ++(*this);
            //     _start = _heh;
            //     _cycle_count = cc;
            // }

            // used for isolate vertex
            if(!_start.is_valid())
                _cycle_count = 0;
        }

    public:
        /* pre-increment */
        VertexOHalfedgeIterBase& operator++()
        {
            if(CCW)
            {
                _heh = _topo->ccw_rotated_halfedge_handle(_heh);
                if(_heh == _start)  
                    ++_cycle_count;
            }
            else
            {
                _heh = _topo->cw_rotated_halfedge_handle(_heh);
                if(_heh == _start)
                    ++_cycle_count;
            }
            return *this;
        }

        /* post-increment */
        VertexOHalfedgeIterBase operator++(int)
        {
            VertexOHalfedgeIterBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement */
        VertexOHalfedgeIterBase& operator--()
        {
            if(CCW)
            {
                if(_heh == _start)  
                    --_cycle_count;
                _heh = _topo->cw_rotated_halfedge_handle(_heh);
            }
            else
            {
                if(_heh == _start)
                    --_cycle_count;
                _heh = _topo->ccw_rotated_halfedge_handle(_heh);
            }
            return *this;
        }

        /* post-decrement */
        VertexOHalfedgeIterBase operator--(int)
        {
            VertexOHalfedgeIterBase copy(*this);
            --(*this);
            return copy;
        }

        /* dereferencing opeartor */
        GraphHalfedgeHandle operator*() const
        {
            return GraphHalfedgeHandle(_heh.idx(), _topo);
        }

        /* pointer operator */
        GraphHalfedgeHandle* operator->() const
        {
            _gheh =  **this;
            return &_gheh;
        }

        /* check whether the iterator is valid in the first circulate */
        bool is_valid() const
        {
            return _heh.is_valid() && _cycle_count == 0;
        }

        bool operator==(const VertexOHalfedgeIterBase& other) const
        {
            return _topo == other._topo && _start == other._start && _heh == other._heh && _cycle_count == other._cycle_count;
        }

        bool operator!=(const VertexOHalfedgeIterBase& other) const
        {
            return !operator==(other);
        }


    public:
        const Topo* _topo;
        HalfedgeHandle _start, _heh;
        int _cycle_count;
        mutable GraphHalfedgeHandle _gheh;
};

//
template<typename Topo, bool CCW>
class VertexEdgeIterBase
{
    public:
        /* default constructor */
        VertexEdgeIterBase() : _topo(0), _cycle_count(0)
        {
        }

        VertexEdgeIterBase(const Topo* topo, VertexHandle vh, bool end = false)
                         : _topo(topo), _start(_topo->halfedge_handle(vh)), _heh(_start), _cycle_count(static_cast<int>(end))
        {
            // if(CCW)
            // {
            //     int cc = _cycle_count;
            //     ++(*this);
            //     _start = _heh;
            //     _cycle_count = cc;
            // }

            // used for isolate vertex
            if(!_start.is_valid())
                _cycle_count = 0;
        }

    public:
        /* pre-increment */
        VertexEdgeIterBase& operator++()
        {
            if(CCW)
            {
                _heh = _topo->ccw_rotated_halfedge_handle(_heh);
                if(_heh == _start)  
                    ++_cycle_count;
            }
            else
            {
                _heh = _topo->cw_rotated_halfedge_handle(_heh);
                if(_heh == _start)
                    ++_cycle_count;
            }
            return *this;
        }

        /* post-increment */
        VertexEdgeIterBase operator++(int)
        {
            VertexEdgeIterBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement */
        VertexEdgeIterBase& operator--()
        {
            if(CCW)
            {
                if(_heh == _start)  
                    --_cycle_count;
                _heh = _topo->cw_rotated_halfedge_handle(_heh);
            }
            else
            {
                if(_heh == _start)
                    --_cycle_count;
                _heh = _topo->ccw_rotated_halfedge_handle(_heh);
            }
            return *this;
        }

        /* post-decrement */
        VertexEdgeIterBase operator--(int)
        {
            VertexEdgeIterBase copy(*this);
            --(*this);
            return copy;
        }

        /* dereferencing opeartor */
        GraphEdgeHandle operator*() const
        {
            return GraphEdgeHandle(_topo->edge_handle(_heh).idx(), _topo);
        }

        /* pointer operator */
        GraphEdgeHandle* operator->() const
        {
            _geh =  **this;
            return &_geh;
        }

        /* check whether the iterator is valid in the first circulate */
        bool is_valid() const
        {
            return _heh.is_valid() && _cycle_count == 0;
        }

        bool operator==(const VertexEdgeIterBase& other) const
        {
            return _topo == other._topo && _start == other._start && _heh == other._heh && _cycle_count == other._cycle_count;
        }

        bool operator!=(const VertexEdgeIterBase& other) const
        {
            return !operator==(other);
        }


    public:
        const Topo* _topo;
        HalfedgeHandle _start, _heh;
        int _cycle_count;
        mutable GraphEdgeHandle _geh;
};

template<typename Topo, bool CCW>
class VertexFaceIterBase
{
    public:
        /* default constructor */
        VertexFaceIterBase() : _topo(0), _cycle_count(0)
        {
        }

        VertexFaceIterBase(const Topo* topo, VertexHandle vh, bool end = false)
                         : _topo(topo), _start(_topo->halfedge_handle(vh)), _heh(_start), _cycle_count(static_cast<int>(end))
        {
            // if(CCW)
            // {
            //     int cc = _cycle_count;
            //     ++(*this);
            //     _start = _heh;
            //     _cycle_count = cc;
            // }

            // used for isolate vertex
            if(!_start.is_valid())
                _cycle_count = 0;

            if(_heh.is_valid() && !_topo->face_handle(_heh).is_valid() && _cycle_count == 0)
                ++(*this);
        }

    public:
        /* pre-increment */
        VertexFaceIterBase& operator++()
        {
            if(CCW)
            {
                do
                {
                    _heh = _topo->ccw_rotated_halfedge_handle(_heh);
                    if(_heh == _start)  
                        ++_cycle_count;
                } while(is_valid() && !_topo->face_handle(_heh).is_valid());
            }
            else
            {
                do
                {
                    _heh = _topo->cw_rotated_halfedge_handle(_heh);
                    if(_heh == _start)  
                        ++_cycle_count;
                } while(is_valid() && !_topo->face_handle(_heh).is_valid());
            }
            return *this;
        }

        /* post-increment */
        VertexFaceIterBase operator++(int)
        {
            VertexFaceIterBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement */
        VertexFaceIterBase& operator--()
        {
            if(CCW)
            {
                do
                {
                    if(_heh == _start)  
                        --_cycle_count;
                    _heh = _topo->cw_rotated_halfedge_handle(_heh);
                } while(is_valid() && !_topo->face_handle(_heh).is_valid());
            }
            else
            {
                do
                {
                    if(_heh == _start)  
                        --_cycle_count;
                    _heh = _topo->ccw_rotated_halfedge_handle(_heh);
                } while(is_valid() && !_topo->face_handle(_heh).is_valid());
            }
            return *this;
        }

        /* post-decrement */
        VertexFaceIterBase operator--(int)
        {
            VertexFaceIterBase copy(*this);
            --(*this);
            return copy;
        }

        /* dereferencing opeartor */
        GraphFaceHandle operator*() const
        {
            return GraphFaceHandle(_topo->face_handle(_heh).idx(), _topo);
        }

        /* pointer operator */
        GraphFaceHandle* operator->() const
        {
            _gfh =  **this;
            return &_gfh;
        }

        /* check whether the iterator is valid in the first circulate */
        bool is_valid() const
        {
            return _heh.is_valid() && _cycle_count == 0;
        }

        bool operator==(const VertexFaceIterBase& other) const
        {
            return _topo == other._topo && _start == other._start && _heh == other._heh && _cycle_count == other._cycle_count;
        }

        bool operator!=(const VertexFaceIterBase& other) const
        {
            return !operator==(other);
        }


    public:
        const Topo* _topo;
        HalfedgeHandle _start, _heh;
        int _cycle_count;
        mutable GraphFaceHandle _gfh;
};

template<typename Topo, bool CCW>
class FaceVertexIterBase
{
    public:
        /* default constructor */
        FaceVertexIterBase() : _topo(0), _cycle_count(0)
        {
        }

        FaceVertexIterBase(const Topo* topo, FaceHandle fh, bool end = false)
                         : _topo(topo), _start(_topo->halfedge_handle(fh)), _heh(_start), _cycle_count(static_cast<int>(end))
        {
            // if(CCW)
            // {
            //     int cc = _cycle_count;
            //     ++(*this);
            //     _start = _heh;
            //     _cycle_count = cc;
            // }
        }

    public:
        /* pre-increment */
        FaceVertexIterBase& operator++()
        {
            if(CCW)
            {
                _heh = _topo->prev_halfedge_handle(_heh);
                if(_heh == _start)  
                    ++_cycle_count;
            }
            else
            {
                _heh = _topo->next_halfedge_handle(_heh);
                if(_heh == _start)
                    ++_cycle_count;
            }
            return *this;
        }

        /* post-increment */
        FaceVertexIterBase operator++(int)
        {
            FaceVertexIterBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement */
        FaceVertexIterBase& operator--()
        {
            if(CCW)
            {
                if(_heh == _start)  
                    --_cycle_count;
                _heh = _topo->prev_halfedge_handle(_heh);
            }
            else
            {
                if(_heh == _start)
                    --_cycle_count;
                _heh = _topo->next_halfedge_handle(_heh);
            }
            return *this;
        }

        /* post-decrement */
        FaceVertexIterBase operator--(int)
        {
            FaceVertexIterBase copy(*this);
            --(*this);
            return copy;
        }

        /* dereferencing opeartor */
        GraphVertexHandle operator*() const
        {
            return GraphVertexHandle(_topo->to_vertex_handle(_heh).idx(), _topo);
        }

        /* pointer operator */
        GraphVertexHandle* operator->() const
        {
            _gvh =  **this;
            return &_gvh;
        }

        /* check whether the iterator is valid in the first circulate */
        bool is_valid() const
        {
            return _heh.is_valid() && _cycle_count == 0;
        }

        bool operator==(const FaceVertexIterBase& other) const
        {
            return _topo == other._topo && _start == other._start && _heh == other._heh && _cycle_count == other._cycle_count;
        }

        bool operator!=(const FaceVertexIterBase& other) const
        {
            return !operator==(other);
        }


    public:
        const Topo* _topo;
        HalfedgeHandle _start, _heh;
        int _cycle_count;
        mutable GraphVertexHandle _gvh;
};

template<typename Topo, bool CCW>
class FaceHalfedgeIterBase
{
    public:
        /* default constructor */
        FaceHalfedgeIterBase() : _topo(0), _cycle_count(0)
        {
        }

        FaceHalfedgeIterBase(const Topo* topo, FaceHandle fh, bool end = false)
                         : _topo(topo), _start(_topo->halfedge_handle(fh)), _heh(_start), _cycle_count(static_cast<int>(end))
        {
            // if(CCW)
            // {
            //     int cc = _cycle_count;
            //     ++(*this);
            //     _start = _heh;
            //     _cycle_count = cc;
            // }
        }

    public:
        /* pre-increment */
        FaceHalfedgeIterBase& operator++()
        {
            if(CCW)
            {
                _heh = _topo->prev_halfedge_handle(_heh);
                if(_heh == _start)  
                    ++_cycle_count;
            }
            else
            {
                _heh = _topo->next_halfedge_handle(_heh);
                if(_heh == _start)
                    ++_cycle_count;
            }
            return *this;
        }

        /* post-increment */
        FaceHalfedgeIterBase operator++(int)
        {
            FaceHalfedgeIterBase copy(*this);
            ++(*this);
            return copy;
        }

        /* pre-decrement */
        FaceHalfedgeIterBase& operator--()
        {
            if(CCW)
            {
                if(_heh == _start)  
                    --_cycle_count;
                _heh = _topo->prev_halfedge_handle(_heh);
            }
            else
            {
                if(_heh == _start)
                    --_cycle_count;
                _heh = _topo->next_halfedge_handle(_heh);
            }
            return *this;
        }

        /* post-decrement */
        FaceHalfedgeIterBase operator--(int)
        {
            FaceHalfedgeIterBase copy(*this);
            --(*this);
            return copy;
        }

        /* dereferencing opeartor */
        GraphHalfedgeHandle operator*() const
        {
            return GraphHalfedgeHandle(_heh.idx(), _topo);
        }

        /* pointer operator */
        GraphHalfedgeHandle* operator->() const
        {
            _gheh =  **this;
            return &_gheh;
        }

        /* check whether the iterator is valid in the first circulate */
        bool is_valid() const
        {
            return _heh.is_valid() && _cycle_count == 0;
        }

        bool operator==(const FaceHalfedgeIterBase& other) const
        {
            return _topo == other._topo && _start == other._start && _heh == other._heh && _cycle_count == other._cycle_count;
        }

        bool operator!=(const FaceHalfedgeIterBase& other) const
        {
            return !operator==(other);
        }


    public:
        const Topo* _topo;
        HalfedgeHandle _start, _heh;
        int _cycle_count;
        mutable GraphHalfedgeHandle _gheh;
};

typedef IteratorBase<GraphTopology, VertexHandle,   GraphVertexHandle>      VertexIter;
typedef IteratorBase<GraphTopology, HalfedgeHandle, GraphHalfedgeHandle>    HalfedgeIter;
typedef IteratorBase<GraphTopology, EdgeHandle,     GraphEdgeHandle>        EdgeIter;
typedef IteratorBase<GraphTopology, FaceHandle,     GraphFaceHandle>        FaceIter;
typedef VertexIter                                                          ConstVertexIter;
typedef HalfedgeIter                                                        ConstHalfedgeIter;
typedef EdgeIter                                                            ConstEdgeIter;
typedef FaceIter                                                            ConstFaceIter;

typedef VertexVertexIterBase<GraphTopology, true>       VertexVertexIter;
typedef VertexVertexIterBase<GraphTopology, true>       VertexVertexCCWIter;
typedef VertexVertexIterBase<GraphTopology, false>      VertexVertexCWIter;
typedef VertexVertexIter                                ConstVertexVertexIter;
typedef VertexVertexCCWIter                             ConstVertexVertexCCWIter;
typedef VertexVertexCWIter                              ConstVertexVertexCWIter;

typedef VertexOHalfedgeIterBase<GraphTopology, true>    VertexOHalfedgeIter;
typedef VertexOHalfedgeIterBase<GraphTopology, true>    VertexOHalfedgeCCWIter;
typedef VertexOHalfedgeIterBase<GraphTopology, false>   VertexOHalfedgeCWIter;
typedef VertexOHalfedgeIter                             ConstVertexOHalfedgeIter;
typedef VertexOHalfedgeCCWIter                          ConstVertexOHalfedgeCCWIter;
typedef VertexOHalfedgeCWIter                           ConstVertexOHalfedgeCWIter;

typedef VertexEdgeIterBase<GraphTopology, true>         VertexEdgeIter;
typedef VertexEdgeIterBase<GraphTopology, true>         VertexEdgeCCWIter;
typedef VertexEdgeIterBase<GraphTopology, false>        VertexEdgeCWIter;
typedef VertexEdgeIter                                  ConstVertexEdgeIter;
typedef VertexEdgeCCWIter                               ConstVertexEdgeCCWIter;
typedef VertexEdgeCWIter                                ConstVertexEdgeCWIter;

typedef VertexFaceIterBase<GraphTopology, true>         VertexFaceIter;
typedef VertexFaceIterBase<GraphTopology, true>         VertexFaceCCWIter;
typedef VertexFaceIterBase<GraphTopology, false>        VertexFaceCWIter;
typedef VertexFaceIter                                  ConstVertexFaceIter;
typedef VertexFaceCCWIter                               ConstVertexFaceCCWIter;
typedef VertexFaceCWIter                                ConstVertexFaceCWIter;

typedef FaceVertexIterBase<GraphTopology, false>        FaceVertexIter;
typedef FaceVertexIterBase<GraphTopology, false>        FaceVertexCCWIter;
typedef FaceVertexIterBase<GraphTopology, true>         FaceVertexCWIter;
typedef FaceVertexIter                                  ConstFaceVertexIter;
typedef FaceVertexCCWIter                               ConstFaceVertexCCWIter;
typedef FaceVertexCWIter                                ConstFaceVertexCWIter;

typedef FaceHalfedgeIterBase<GraphTopology, false>      FaceHalfedgeIter;
typedef FaceHalfedgeIterBase<GraphTopology, false>      FaceHalfedgeCCWIter;
typedef FaceHalfedgeIterBase<GraphTopology, true>       FaceHalfedgeCWIter;
typedef FaceHalfedgeIter                                ConstFaceHalfedgeIter;
typedef FaceHalfedgeCCWIter                             ConstFaceHalfedgeCCWIter;
typedef FaceHalfedgeCWIter                              ConstFaceHalfedgeCWIter;


/**
 * @brief base struct that store the mesh handle connectivity information.
 */
class GraphTopology
{
    public:
        GraphTopology() 
        {
        };

        virtual ~GraphTopology()
        {
        }

    public:
        /* get the number of vertices */
        unsigned n_vertices() const
        {
            return _vertices.size();
        }

        /* get the number of edges */
        unsigned n_edges() const
        {
            return _edges.size();
        }

        /* get the number of vertices */
        unsigned n_halfedges() const
        {
            return _edges.size() * 2;
        }

        /* get the number of faces */
        unsigned n_faces() const
        {
            return _faces.size();
        }

        /* clear all elements */
        void clear()
        {
            _vertices.clear();
            _edges.clear();
            _faces.clear();
        }

        /* get i'th graph vertex */
        GraphVertexHandle vertex(VertexHandle vh) const
        {
            assert(vh.is_valid() && vh.idx() < (int)n_vertices());
            return GraphVertexHandle(vh.idx(), this);
        }
    
        /* get i'th vertex handle */
        VertexHandle vertex_handle(unsigned i) const
        {
            assert(i < n_vertices());
            return VertexHandle(i);
        }

        /* get the destination of the halfedge */
        VertexHandle to_vertex_handle(HalfedgeHandle heh) const
        {
            return halfedge_item(heh)._vertex_handle;
        }

        /* get the destination of the halfedge */
        VertexHandle from_vertex_handle(HalfedgeHandle heh) const
        {
            return to_vertex_handle(opposite_halfedge_handle(heh));
        }

        /* get i'th graph halfedge */
        GraphHalfedgeHandle halfedge(HalfedgeHandle heh) const
        {
            assert(heh.is_valid() && heh.idx() < (int)n_halfedges());
            return GraphHalfedgeHandle(heh.idx(), this);
        }

        /* get i'th halfedge handle */
        HalfedgeHandle halfedge_handle(unsigned i) const
        {
            assert(i < n_halfedges());
            return HalfedgeHandle(i);
        }

        /* get the handle of a vertex's outgoing halfedge */
        HalfedgeHandle halfedge_handle(VertexHandle vh) const
        {
            return vertex_item(vh)._halfedge_handle;
        }

        /* get halfedge handle with edge handle and a side */
        HalfedgeHandle halfedge_handle(EdgeHandle eh, unsigned i) const
        {
            assert(eh.is_valid() && eh.idx() < (int)n_edges() && i <= 1);
            return HalfedgeHandle((eh.idx() << 1) + i);
        }

        /* get halfedge handle of a face */
        HalfedgeHandle halfedge_handle(FaceHandle fh) const
        {
            return face_item(fh)._halfedge_handle;
        }

        /* get handle of the opposite halfedge */
        HalfedgeHandle opposite_halfedge_handle(HalfedgeHandle heh) const
        {
            assert(heh.is_valid() && heh.idx() < (int)n_halfedges());
            return HalfedgeHandle(heh.idx() ^ 1);
        }

        /* get handle of the previous halfedge */
        HalfedgeHandle prev_halfedge_handle(HalfedgeHandle heh) const
        {
            return halfedge_item(heh)._prev_halfedge_handle;
        }

        /* get handle of the next halfedge */
        HalfedgeHandle next_halfedge_handle(HalfedgeHandle heh) const
        {
            return halfedge_item(heh)._next_halfedge_handle;
        }

        /* get the first halfedge handle in the clock-wise order */
        HalfedgeHandle cw_rotated_halfedge_handle(HalfedgeHandle heh) const
        {
            return next_halfedge_handle(opposite_halfedge_handle(heh));
        }

        /* get the first halfedge handle in the countor-clock-wise order */
        HalfedgeHandle ccw_rotated_halfedge_handle(HalfedgeHandle heh) const
        {
            return opposite_halfedge_handle(prev_halfedge_handle(heh));
        }

        /* get i'th graph edge */
        GraphEdgeHandle edge(EdgeHandle eh) const
        {
            assert(eh.is_valid() && eh.idx() < (int)n_edges());
            return GraphEdgeHandle(eh.idx(), this);
        }

        /* get i'th edge handle */
        EdgeHandle edge_handle(unsigned i) const
        {
            assert(i < n_edges());
            return EdgeHandle(i);
        }

        /* get i'th edge handle */
        EdgeHandle edge_handle(HalfedgeHandle heh) const
        {
            assert(heh.is_valid() && heh.idx() < (int)n_halfedges());
            return EdgeHandle(heh.idx() >> 1);
        }

        /* get i'th graph face */
        GraphFaceHandle face(FaceHandle fh) const
        {
            assert(fh.is_valid() && fh.idx() < (int)n_faces());
            return GraphFaceHandle(fh.idx(), this);
        }

        /* get i'th face handle */
        FaceHandle face_handle(unsigned i) const
        {
            assert(i < n_faces());
            return FaceHandle(i);
        }

        /* get face handle the halfedge lies on */
        FaceHandle face_handle(HalfedgeHandle heh) const
        {
            return halfedge_item(heh)._face_handle;
        }

    public:
        /* begin iterator for vertices */
        VertexIter vertices_begin()
        {
            return VertexIter(this, VertexHandle(0));
        }

        /* const begin iterator for vertices */
        ConstVertexIter vertices_begin() const
        {
            return ConstVertexIter(this, VertexHandle(0));
        }

        /* end iterator for vertices */
        VertexIter vertices_end()
        {
            return VertexIter(this, VertexHandle(int(n_vertices())));
        }

        /* const end iterator for vertices */
        ConstVertexIter vertices_end() const
        {
            return ConstVertexIter(this, VertexHandle(int(n_vertices())));
        }

        /* begin iterator for halfedges */
        HalfedgeIter halfedges_begin()
        {
            return HalfedgeIter(this, HalfedgeHandle(0));
        }

        /* const begin iterator for halfedges */
        ConstHalfedgeIter halfedges_begin() const
        {
            return ConstHalfedgeIter(this, HalfedgeHandle(0));
        }

        /* end iterator for halfedges */
        HalfedgeIter halfedges_end()
        {
            return HalfedgeIter(this, HalfedgeHandle(int(n_halfedges())));
        }

        /* const end iterator for halfedges */
        ConstHalfedgeIter halfedges_end() const
        {
            return ConstHalfedgeIter(this, HalfedgeHandle(int(n_halfedges())));
        }

        /* begin iterator for edges */
        EdgeIter edges_begin()
        {
            return EdgeIter(this, EdgeHandle(0));
        }

        /* const begin iterator for edges */
        ConstEdgeIter edges_begin() const
        {
            return ConstEdgeIter(this, EdgeHandle(0));
        }

        /* end iterator for edges */
        EdgeIter edges_end()
        {
            return EdgeIter(this, EdgeHandle(int(n_edges())));
        }

        /* const end iterator for edges */
        ConstEdgeIter edges_end() const
        {
            return ConstEdgeIter(this, EdgeHandle(int(n_edges())));
        }

        /* begin iterator for faces */
        FaceIter faces_begin()
        {
            return FaceIter(this, FaceHandle(0));
        }

        /* const begin iterator for faces */
        ConstFaceIter faces_begin() const
        {
            return ConstFaceIter(this, FaceHandle(0));
        }

        /* end iterator for faces */
        FaceIter faces_end()
        {
            return FaceIter(this, FaceHandle(int(n_faces())));
        }

        /* const end iterator for faces */
        ConstFaceIter faces_end() const
        {
            return ConstFaceIter(this, FaceHandle(int(n_faces())));
        }

        /* vertex-vertex circulator */
        VertexVertexIter vv_begin(VertexHandle vh)
        {
            return VertexVertexIter(this, vh);
        }

        /* vertex-vertex circulator */
        VertexVertexIter vv_end(VertexHandle vh)
        {
            return VertexVertexIter(this, vh, true);
        }

        /* const vertex-vertex circulator */
        ConstVertexVertexIter vv_begin(VertexHandle vh) const
        {
            return ConstVertexVertexIter(this, vh);
        }

        /* const vertex-vertex circulator */
        ConstVertexVertexIter vv_end(VertexHandle vh) const
        {
            return ConstVertexVertexIter(this, vh, true);
        }

        /* vertex-vertex contour clock wise circulator */
        VertexVertexCCWIter vv_ccwbegin(VertexHandle vh)
        {
            return VertexVertexCCWIter(this, vh);
        }

        /* vertex-vertex contour clock wise circulator */
        VertexVertexCCWIter vv_ccwend(VertexHandle vh)
        {
            return VertexVertexCCWIter(this, vh, true);
        }

        /* const vertex-vertex contour clock wise circulator */
        ConstVertexVertexCCWIter vv_ccwbegin(VertexHandle vh) const
        {
            return ConstVertexVertexCCWIter(this, vh);
        }

        /* const vertex-vertex contour clock wise circulator */
        ConstVertexVertexCCWIter vv_ccwend(VertexHandle vh) const
        {
            return ConstVertexVertexCCWIter(this, vh, true);
        }

        /* vertex-vertex clock wise circulator */
        VertexVertexCWIter vv_cwbegin(VertexHandle vh)
        {
            return VertexVertexCWIter(this, vh);
        }

        /* vertex-vertex clock wise circulator */
        VertexVertexCWIter vv_cwend(VertexHandle vh)
        {
            return VertexVertexCWIter(this, vh, true);
        }

        /* const vertex-vertex clock wise circulator */
        ConstVertexVertexCWIter vv_cwbegin(VertexHandle vh) const
        {
            return ConstVertexVertexCWIter(this, vh);
        }

        /* const vertex-vertex clock wise circulator */
        ConstVertexVertexCWIter vv_cwend(VertexHandle vh) const
        {
            return ConstVertexVertexCWIter(this, vh, true);
        }

        /* vertex - outgoing halfedge circulator */
        VertexOHalfedgeIter voh_begin(VertexHandle vh)
        {
            return VertexOHalfedgeIter(this, vh);
        }

        /* vertex - outgoing halfedge circulator */
        VertexOHalfedgeIter voh_end(VertexHandle vh)
        {
            return VertexOHalfedgeIter(this, vh, true);
        }

        /* const vertex - outgoing halfedge circulator */
        ConstVertexOHalfedgeIter voh_begin(VertexHandle vh) const
        {
            return ConstVertexOHalfedgeIter(this, vh);
        }

        /* const vertex - outgoing halfedge circulator */
        ConstVertexOHalfedgeIter voh_end(VertexHandle vh) const
        {
            return ConstVertexOHalfedgeIter(this, vh, true);
        }

        /* vertex - outgoing halfedge contour clock wise circulator */
        VertexOHalfedgeCCWIter voh_ccwbegin(VertexHandle vh)
        {
            return VertexOHalfedgeCCWIter(this, vh);
        }

        /* vertex - outgoing halfedge contour clock wise circulator */
        VertexOHalfedgeCCWIter voh_ccwend(VertexHandle vh)
        {
            return VertexOHalfedgeCCWIter(this, vh, true);
        }

        /* const vertex - outgoing halfedge contour clock wise circulator */
        ConstVertexOHalfedgeCCWIter voh_ccwbegin(VertexHandle vh) const
        {
            return ConstVertexOHalfedgeCCWIter(this, vh);
        }

        /* const vertex - outgoing halfedge contour clock wise circulator */
        ConstVertexOHalfedgeCCWIter voh_ccwend(VertexHandle vh) const
        {
            return ConstVertexOHalfedgeCCWIter(this, vh, true);
        }

        /* vertex - outgoing halfedge clock wise circulator */
        VertexOHalfedgeCWIter voh_cwbegin(VertexHandle vh)
        {
            return VertexOHalfedgeCWIter(this, vh);
        }

        /* vertex - outgoing halfedge clock wise circulator */
        VertexOHalfedgeCWIter voh_cwend(VertexHandle vh)
        {
            return VertexOHalfedgeCWIter(this, vh, true);
        }

        /* const vertex - outgoing halfedge clock wise circulator */
        ConstVertexOHalfedgeCWIter voh_cwbegin(VertexHandle vh) const
        {
            return ConstVertexOHalfedgeCWIter(this, vh);
        }

        /* const vertex - outgoing halfedge clock wise circulator */
        ConstVertexOHalfedgeCWIter voh_cwend(VertexHandle vh) const
        {
            return ConstVertexOHalfedgeCWIter(this, vh, true);
        }

        /* vertex face circulator */
        VertexFaceIter vf_begin(VertexHandle vh)
        {
            return VertexFaceIter(this, vh);
        }

        /* vertex face circulator */
        VertexFaceIter vf_end(VertexHandle vh)
        {
            return VertexFaceIter(this, vh, true);
        }

        /* const vertex face circulator */
        ConstVertexFaceIter vf_begin(VertexHandle vh) const
        {
            return ConstVertexFaceIter(this, vh);
        }

        /* const vertex face circulator */
        ConstVertexFaceIter vf_end(VertexHandle vh) const
        {
            return ConstVertexFaceIter(this, vh, true);
        }

        /* vertex face contour clock wise circulator */
        VertexFaceCCWIter vf_ccwbegin(VertexHandle vh)
        {
            return VertexFaceCCWIter(this, vh);
        }

        /* vertex face contour clock wise circulator */
        VertexFaceCCWIter vf_ccwend(VertexHandle vh)
        {
            return VertexFaceCCWIter(this, vh, true);
        }

        /* const vertex face contour clock wise circulator */
        ConstVertexFaceCCWIter vf_ccwbegin(VertexHandle vh) const
        {
            return ConstVertexFaceCCWIter(this, vh);
        }

        /* const vertex face contour clock wise circulator */
        ConstVertexFaceCCWIter vf_ccwend(VertexHandle vh) const
        {
            return ConstVertexFaceCCWIter(this, vh, true);
        }

        /* vertex face clock wise circulator */
        VertexFaceCWIter vf_cwbegin(VertexHandle vh)
        {
            return VertexFaceCWIter(this, vh);
        }

        /* vertex face clock wise circulator */
        VertexFaceCWIter vf_cwend(VertexHandle vh)
        {
            return VertexFaceCWIter(this, vh, true);
        }

        /* const vertex face clock wise circulator */
        ConstVertexFaceCWIter vf_cwbegin(VertexHandle vh) const
        {
            return ConstVertexFaceCWIter(this, vh);
        }

        /* const vertex face clock wise circulator */
        ConstVertexFaceCWIter vf_cwend(VertexHandle vh) const
        {
            return ConstVertexFaceCWIter(this, vh, true);
        }

        /* face vertex circulator */
        FaceVertexIter fv_begin(FaceHandle fh)
        {
            return FaceVertexIter(this, fh);
        }

        /* face vertex circulator */
        FaceVertexIter fv_end(FaceHandle fh)
        {
            return FaceVertexIter(this, fh, true);
        }

        /* const face vertex circulator */
        ConstFaceVertexIter fv_begin(FaceHandle fh) const
        {
            return ConstFaceVertexIter(this, fh);
        }

        /* const face vertex circulator */
        ConstFaceVertexIter fv_end(FaceHandle fh) const
        {
            return ConstFaceVertexIter(this, fh, true);
        }

        /* face vertex contour clock wise circulator */
        FaceVertexCCWIter fv_ccwbegin(FaceHandle fh)
        {
            return FaceVertexCCWIter(this, fh);
        }

        /* face vertex contour clock wise circulator */
        FaceVertexCCWIter fv_ccwend(FaceHandle fh)
        {
            return FaceVertexCCWIter(this, fh, true);
        }

        /* const face vertex contour clock wise circulator */
        ConstFaceVertexCCWIter fv_ccwbegin(FaceHandle fh) const
        {
            return ConstFaceVertexCCWIter(this, fh);
        }

        /* const face vertex contour clock wise circulator */
        ConstFaceVertexCCWIter fv_ccwend(FaceHandle fh) const
        {
            return ConstFaceVertexCCWIter(this, fh, true);
        }

        /* face vertex clock wise circulator */
        FaceVertexCWIter fv_cwbegin(FaceHandle fh)
        {
            return FaceVertexCWIter(this, fh);
        }

        /* face vertex clock wise circulator */
        FaceVertexCWIter fv_cwend(FaceHandle fh)
        {
            return FaceVertexCWIter(this, fh, true);
        }

        /* const face vertex clock wise circulator */
        ConstFaceVertexCWIter fv_cwbegin(FaceHandle fh) const
        {
            return ConstFaceVertexCWIter(this, fh);
        }

        /* const face vertex clock wise circulator */
        ConstFaceVertexCWIter fv_cwend(FaceHandle fh) const
        {
            return ConstFaceVertexCWIter(this, fh, true);
        }

        /* face halfedge circulator */
        FaceHalfedgeIter fh_begin(FaceHandle fh)
        {
            return FaceHalfedgeIter(this, fh);
        }

        /* face halfedge circulator */
        FaceHalfedgeIter fh_end(FaceHandle fh)
        {
            return FaceHalfedgeIter(this, fh, true);
        }

        /* const face halfedge circulator */
        ConstFaceHalfedgeIter fh_begin(FaceHandle fh) const
        {
            return ConstFaceHalfedgeIter(this, fh);
        }

        /* const face halfedge circulator */
        ConstFaceHalfedgeIter fh_end(FaceHandle fh) const
        {
            return ConstFaceHalfedgeIter(this, fh, true);
        }

        /* face halfedge contour clock wise circulator */
        FaceHalfedgeCCWIter fh_ccwbegin(FaceHandle fh)
        {
            return FaceHalfedgeCCWIter(this, fh);
        }

        /* face halfedge contour clock wise circulator */
        FaceHalfedgeCCWIter fh_ccwend(FaceHandle fh)
        {
            return FaceHalfedgeCCWIter(this, fh, true);
        }

        /* const face halfedge contour clock wise circulator */
        ConstFaceHalfedgeCCWIter fh_ccwbegin(FaceHandle fh) const
        {
            return ConstFaceHalfedgeCCWIter(this, fh);
        }

        /* const face halfedge contour clock wise circulator */
        ConstFaceHalfedgeCCWIter fh_ccwend(FaceHandle fh) const
        {
            return ConstFaceHalfedgeCCWIter(this, fh, true);
        }

        /* face halfedge clock wise circulator */
        FaceHalfedgeCWIter fh_cwbegin(FaceHandle fh)
        {
            return FaceHalfedgeCWIter(this, fh);
        }

        /* face halfedge clock wise circulator */
        FaceHalfedgeCWIter fh_cwend(FaceHandle fh)
        {
            return FaceHalfedgeCWIter(this, fh, true);
        }

        /* const face halfedge clock wise circulator */
        ConstFaceHalfedgeCWIter fh_cwbegin(FaceHandle fh) const
        {
            return ConstFaceHalfedgeCWIter(this, fh);
        }

        /* const face halfedge clock wise circulator */
        ConstFaceHalfedgeCWIter fh_cwend(FaceHandle fh) const
        {
            return ConstFaceHalfedgeCWIter(this, fh, true);
        }

    public:
        /* check if the vertex is a boundary vertex */
        bool is_boundary(VertexHandle vh) const
        {
            HalfedgeHandle heh(halfedge_handle(vh));
            return !(heh.is_valid() && face_handle(heh).is_valid());
        }

        /* check if a halfedge is a boundary halfedge */
        bool is_boundary(HalfedgeHandle heh) const
        {
            return !face_handle(heh).is_valid();
        }

        /* check if a edge is a boundary edge */
        bool is_boundary(EdgeHandle eh) const
        {
            return is_boundary(halfedge_handle(eh,0)) || is_boundary(halfedge_handle(eh,1));
        }

        /* number of vertices around given vertex */
        unsigned degree(VertexHandle vh) const
        {
            unsigned count(0);
            for(auto vv = vv_begin(vh); vv != vv_end(vh); ++vv)
                ++count;
            return count;
        }

        /* number of vertices make up this face */
        unsigned degree(FaceHandle fh) const
        {
            unsigned count(0);
            for(auto fv = fv_begin(fh); fv != fv_end(fh); ++fv)
                ++count;
            return count;
        }

        /* return the maximum vertex degree*/
        unsigned max_vertex_degree() const
        {
            unsigned d(0);
            for(auto vit = this->vertices_begin(); vit != this->vertices_end(); ++vit)
                d = std::max(d, degree(*vit));
            return d;
        }

        /* return the maximum face degree */
        unsigned max_face_degree() const
        {
            unsigned d(0);
            for(auto fit = this->faces_begin(); fit != this->faces_end(); ++fit)
                d = std::max(d, degree(*fit));
            return d;
        }

        /* check whether all the face degree equal d */
        bool has_constant_face_degree(unsigned d) const
        {
            for(auto fit = this->faces_begin(); fit != this->faces_end(); ++fit)
                if(degree(*fit) != d)
                    return false;
            return true;
        }

    public:
        /* check whether flip eh is topologically correct */
        bool is_flip_ok(EdgeHandle eh) const
        {
            if(!eh.is_valid()) 
                return false;

            // boundary edges can not be flipped
            if(is_boundary(eh))
                return false;
            
            HalfedgeHandle h0 = halfedge_handle(eh, 0);
            HalfedgeHandle h1 = halfedge_handle(eh, 1);

            // neighbor faces must have degree 3
            if(next_halfedge_handle(next_halfedge_handle(h0)) != prev_halfedge_handle(h0) || 
               next_halfedge_handle(next_halfedge_handle(h1)) != prev_halfedge_handle(h1) )
                return false;

            VertexHandle va = to_vertex_handle(next_halfedge_handle(h0));
            VertexHandle vb = to_vertex_handle(next_halfedge_handle(h1));

            if(va == vb)
                return false;
            
            for(auto vv_it = vv_begin(va); vv_it != vv_end(va); ++vv_it)
                if(*vv_it == vb)
                    return false;
            
            return true;
        }

        void flip(EdgeHandle eh)
        {
            assert(is_flip_ok(eh));
            
            HalfedgeHandle a0 = halfedge_handle(eh, 0);
            HalfedgeHandle a1 = next_halfedge_handle(a0);
            HalfedgeHandle a2 = prev_halfedge_handle(a0);
            HalfedgeHandle b0 = halfedge_handle(eh, 1);
            HalfedgeHandle b1 = next_halfedge_handle(b0);
            HalfedgeHandle b2 = prev_halfedge_handle(b0);

            VertexHandle v0 = to_vertex_handle(b0);
            VertexHandle v1 = to_vertex_handle(a0);
            VertexHandle va = to_vertex_handle(a1);
            VertexHandle vb = to_vertex_handle(b1);

            FaceHandle fa = face_handle(a0);
            FaceHandle fb = face_handle(b0);

            halfedge_item(a0)._vertex_handle = va;
            halfedge_item(a0)._next_halfedge_handle = a2;
            halfedge_item(a0)._prev_halfedge_handle = b1;

            halfedge_item(a1)._next_halfedge_handle = b0;
            halfedge_item(a1)._prev_halfedge_handle = b2;
            halfedge_item(a1)._face_handle = fb;

            halfedge_item(a2)._next_halfedge_handle = b1;
            halfedge_item(a2)._prev_halfedge_handle = a0;

            halfedge_item(b0)._vertex_handle = vb;
            halfedge_item(b0)._next_halfedge_handle = b2;
            halfedge_item(b0)._prev_halfedge_handle = a1;

            halfedge_item(b1)._next_halfedge_handle = a0;
            halfedge_item(b1)._prev_halfedge_handle = a2;
            halfedge_item(b1)._face_handle = fa;

            halfedge_item(b2)._next_halfedge_handle = a1;
            halfedge_item(b2)._prev_halfedge_handle = b0;

            face_item(fa)._halfedge_handle = a0;
            face_item(fb)._halfedge_handle = b0;

            if(halfedge_handle(v0) == a0)
                vertex_item(v0)._halfedge_handle = b1;
            if(halfedge_handle(v1) == b0)
                vertex_item(v1)._halfedge_handle = a1;
        }

    public:
        /* add a new vertex */
        VertexHandle new_vertex()
        {
            _vertices.push_back(VertexItem());
            return VertexHandle(_vertices.size() - 1);
        }

        /* add a new edge */
        HalfedgeHandle new_edge(VertexHandle start, VertexHandle end)
        {
            _edges.push_back(EdgeItem());
            EdgeHandle eh(_edges.size() - 1);
            HalfedgeHandle he0 = halfedge_handle(eh, 0);
            HalfedgeHandle he1 = halfedge_handle(eh, 1);
            halfedge_item(he0)._vertex_handle = end;
            halfedge_item(he1)._vertex_handle = start;
            return he0;
        }

        /* add a new face */
        FaceHandle new_face()
        {
            _faces.push_back(FaceItem());
            return FaceHandle(_faces.size() - 1);
        }

        /**
         * @brief find halfedge with corresponding end points
         * @param start end vertex of halfedge 
         * @param end end vertex of halfedge 
         */
        GraphHalfedgeHandle find_halfedge(VertexHandle start, VertexHandle end)
        {
            assert(start.is_valid() && end.is_valid());
            for(ConstVertexOHalfedgeIter voh = voh_begin(start); voh.is_valid(); ++voh)
            {
                if(to_vertex_handle(*voh) == end)
                    return *voh;
            }
            return GraphHalfedgeHandle(-1, this);
        }

        /* if the vertex has a boundary outgoing halfedge around it, link the halfedge */
        void adjust_outgoing_halfedge(VertexHandle vh)
        {
            for(ConstVertexOHalfedgeIter voh_it = voh_begin(vh); voh_it != voh_end(vh); ++voh_it)
            {
                if(is_boundary(*voh_it))
                {
                    vertex_item(vh)._halfedge_handle = *voh_it;
                    break;
                }
            }
        }

    public:
        /**
         * @brief add a vertex into graph 
         * @return the new vertex handle
         */
        GraphVertexHandle add_vertex()
        {
            return vertex(new_vertex());
        }

        /**
         * @brief add a 3-degree face into graph and build connectivity information
         * @return the new face handle 
         */
        GraphFaceHandle add_face(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2)
        {
            return add_face(std::vector<VertexHandle>{vh0, vh1, vh2});
        }

        /**
         * @brief add a 4-degree face into graph and build connectivity information
         * @return the new face handle 
         */
        GraphFaceHandle add_face(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2, VertexHandle vh3)
        {
            return add_face(std::vector<VertexHandle>{vh0, vh1, vh2, vh3});
        }

        /**
         * @brief add a face into graph and build connectivity information
         * @return the new face handle 
         */
        GraphFaceHandle add_face(const std::vector<VertexHandle>& vhs)
        {
            unsigned n = vhs.size();
            assert(n > 2);

            if(_tmp_edge_storage.size() < n)
            {
                _tmp_edge_storage.resize(n);
                _he_link_storage.resize(6*n);
            }
             
            for(unsigned i = 0, j = 1; i < n; ++i, ++j, j%=n)
            {
                if(!is_boundary(vhs[i]))
                {
                    std::cerr << "Warning : Complex vertex" << std::endl;
                    return GraphFaceHandle(-1, this);
                }
                _tmp_edge_storage[i].halfedge_handle = find_halfedge(vhs[i], vhs[j]);
                _tmp_edge_storage[i].is_new = !_tmp_edge_storage[i].halfedge_handle.is_valid();
                if(!_tmp_edge_storage[i].is_new && !is_boundary(_tmp_edge_storage[i].halfedge_handle))
                {
                    std::cerr << "Warning : Complex edge" << std::endl;
                    return GraphFaceHandle(-1, this);
                }
            }

            unsigned link_count = 0;
            for(unsigned i = 0, j = 1; i < n; ++i, ++j, j%=n)
            {
                if(!_tmp_edge_storage[i].is_new && !_tmp_edge_storage[j].is_new)
                {
                    HalfedgeHandle inner_prev_he = _tmp_edge_storage[i].halfedge_handle;
                    HalfedgeHandle inner_next_he = _tmp_edge_storage[j].halfedge_handle;

                    if(next_halfedge_handle(inner_prev_he) != inner_next_he)
                    {
                        HalfedgeHandle outer_prev_he = opposite_halfedge_handle(inner_next_he);
                        HalfedgeHandle outer_next_he = opposite_halfedge_handle(inner_prev_he);
                        HalfedgeHandle boundary_prev_he = outer_prev_he;
                        do 
                        {
                            boundary_prev_he = opposite_halfedge_handle(next_halfedge_handle(boundary_prev_he));
                        }
                        while(!is_boundary(boundary_prev_he));
                        HalfedgeHandle boundary_next_he = next_halfedge_handle(boundary_prev_he);

                        if(boundary_prev_he == inner_prev_he)
                        {
                            std::cerr << "Warning : relink failed" << std::endl;
                            return GraphFaceHandle(-1, this);
                        }

                        assert(is_boundary(boundary_prev_he));
                        assert(is_boundary(boundary_next_he));

                        HalfedgeHandle patch_start_he = next_halfedge_handle(inner_prev_he);
                        HalfedgeHandle patch_end_he   = prev_halfedge_handle(inner_next_he);

                        assert(boundary_prev_he.is_valid());
                        assert(boundary_next_he.is_valid());
                        assert(patch_start_he.is_valid());
                        assert(patch_end_he.is_valid());
                        assert(inner_prev_he.is_valid());
                        assert(inner_next_he.is_valid());

                        _he_link_storage[link_count++] = std::make_pair(boundary_prev_he, patch_start_he);
                        _he_link_storage[link_count++] = std::make_pair(patch_end_he, boundary_next_he);
                        _he_link_storage[link_count++] = std::make_pair(inner_prev_he, inner_next_he);
                    }
                }
            }

            for(unsigned i = 0, j = 1; i < n; ++i, ++j, j%=n)
            {
                if(_tmp_edge_storage[i].is_new)
                    _tmp_edge_storage[i].halfedge_handle = new_edge(vhs[i], vhs[j]);
            }

            FaceHandle fh(new_face());
            face_item(fh)._halfedge_handle = _tmp_edge_storage[n-1].halfedge_handle;

            for(unsigned i = 0, j = 1; i < n; ++i, ++j, j%=n)
            {
                VertexHandle vh = vhs[j];
                HalfedgeHandle inner_prev_he = _tmp_edge_storage[i].halfedge_handle;
                HalfedgeHandle inner_next_he = _tmp_edge_storage[j].halfedge_handle;
                assert(inner_prev_he.is_valid());
                assert(inner_next_he.is_valid());

                unsigned flag = 0;
                if(_tmp_edge_storage[i].is_new) flag |= 1;
                if(_tmp_edge_storage[j].is_new) flag |= 2;

                if(flag)
                {
                    HalfedgeHandle outer_prev_he = opposite_halfedge_handle(inner_next_he);
                    HalfedgeHandle outer_next_he = opposite_halfedge_handle(inner_prev_he);
                    assert(outer_prev_he.is_valid());
                    assert(outer_next_he.is_valid());
                    switch(flag)
                    {
                        case 1:
                        {
                            HalfedgeHandle boundary_prev_he = prev_halfedge_handle(inner_next_he);
                            assert(boundary_prev_he.is_valid());
                            _he_link_storage[link_count++] = std::make_pair(boundary_prev_he, outer_next_he);
                            vertex_item(vh)._halfedge_handle = outer_next_he;
                            break;
                        }
                        case 2:
                        {
                            HalfedgeHandle boundary_next_he = next_halfedge_handle(inner_prev_he);
                            assert(boundary_next_he.is_valid());
                            _he_link_storage[link_count++] = std::make_pair(outer_prev_he, boundary_next_he);
                            vertex_item(vh)._halfedge_handle = boundary_next_he;
                            break;
                        }
                        case 3:
                        {
                            if(!halfedge_handle(vh).is_valid())
                            {
                                vertex_item(vh)._halfedge_handle = outer_next_he;
                                _he_link_storage[link_count++] = std::make_pair(outer_prev_he, outer_next_he);
                            }
                            else
                            {
                                HalfedgeHandle boundary_next_he = halfedge_handle(vh);
                                HalfedgeHandle boundary_prev_he = prev_halfedge_handle(boundary_next_he);
                                assert(boundary_next_he.is_valid());
                                assert(boundary_prev_he.is_valid());
                                _he_link_storage[link_count++] = std::make_pair(boundary_prev_he, outer_next_he);
                                _he_link_storage[link_count++] = std::make_pair(outer_prev_he, boundary_next_he);
                            }
                            break;
                        }
                    }
                    _he_link_storage[link_count++] = std::make_pair(inner_prev_he, inner_next_he);
                }
                else
                {
                    _tmp_edge_storage[j].need_adjust = (halfedge_handle(vh) == inner_next_he);
                }

                halfedge_item(_tmp_edge_storage[i].halfedge_handle)._face_handle = fh;
            }

            for(unsigned i = 0; i < link_count; ++i)
            {
                halfedge_item(_he_link_storage[i].first)._next_halfedge_handle = _he_link_storage[i].second;
                halfedge_item(_he_link_storage[i].second)._prev_halfedge_handle = _he_link_storage[i].first;
            }

            for(unsigned i = 0; i < n; ++i)
            {
                if(_tmp_edge_storage[i].need_adjust)
                    adjust_outgoing_halfedge(vhs[i]);
            }

            return face(fh);
        }

    public:
        /* print the halfedge items */
        void print()
        {
            printf("vertex  outgoing_halfedge\n");
            for(unsigned i = 0; i < n_vertices(); ++i)
            {
                printf("%-8d%d\n", i, vertex_item(vertex_handle(i))._halfedge_handle.idx());
            }

            printf("halfedge from_v to_v next_he prev_he oppo_he face\n");
            for(unsigned i = 0; i < n_halfedges(); ++i)
            {
                printf("%-9d%-7d%-5d%-8d%-8d%-8d%d\n", i, from_vertex_handle(halfedge_handle(i)).idx(), to_vertex_handle(halfedge_handle(i)).idx(),
                                            halfedge_item(halfedge_handle(i))._next_halfedge_handle.idx(), halfedge_item(halfedge_handle(i))._prev_halfedge_handle.idx(),
                                            opposite_halfedge_handle(halfedge_handle(i)).idx(), halfedge_item(halfedge_handle(i))._face_handle.idx());
            }
        }

    
    private:
        struct AddFaceEdgeStorage
        {
            HalfedgeHandle halfedge_handle;
            bool is_new;
            bool need_adjust;
        };
        /* container that store the edge information when add a new face */
        std::vector<AddFaceEdgeStorage> _tmp_edge_storage;
        /* container that store the information used for next halfedge link when add a new face */
        std::vector<std::pair<HalfedgeHandle, HalfedgeHandle> > _he_link_storage;
        
    protected:
        /* use vertex handle to get the vertex item */
        VertexItem& vertex_item(VertexHandle vh)
        {
            assert(vh.is_valid() && vh.idx() < n_vertices());
            return _vertices[vh.idx()];
        }

        /* use vertex handle to get the vertex item */
        const VertexItem& vertex_item(VertexHandle vh) const
        {
            assert(vh.is_valid() && vh.idx() < n_vertices());
            return _vertices[vh.idx()];
        }

        /* use halfedge handle to get the halfedge item */
        HalfedgeItem& halfedge_item(HalfedgeHandle heh)
        {
            assert(heh.is_valid() && heh.idx() < n_halfedges());
            return _edges[heh.idx() >> 1]._halfedges[heh.idx() & 1];
        }

        /* use halfedge handle to get the halfedge item */
        const HalfedgeItem& halfedge_item(HalfedgeHandle heh) const
        {
            assert(heh.is_valid() && heh.idx() < (int)n_halfedges());
            return _edges[heh.idx() >> 1]._halfedges[heh.idx() & 1];
        }

        /* use edge handle to get the edge item */
        EdgeItem& edge_item(EdgeHandle eh)
        {
            assert(eh.is_valid() && eh.idx() < n_edges());
            return _edges[eh.idx()];
        }

        /* use edge handle to get the edge item */
        const EdgeItem& edge_item(EdgeHandle eh) const
        {
            assert(eh.is_valid() && eh.idx() < n_edges());
            return _edges[eh.idx()];
        }
        
        /* use face handle to get the face item */
        FaceItem& face_item(FaceHandle fh)
        {
            assert(fh.is_valid() && fh.idx() < n_faces());
            return _faces[fh.idx()];
        }

        /* use face handle to get the face item */
        const FaceItem& face_item(FaceHandle fh) const
        {
            assert(fh.is_valid() && fh.idx() < n_faces());
            return _faces[fh.idx()];
        }

        /* vertex elements */
        std::vector<VertexItem> _vertices;

        /* edge elements */
        std::vector<EdgeItem>   _edges;

        /* face elements */
        std::vector<FaceItem>   _faces;
};

/* GraphVertexHandle make smart*/
GraphHalfedgeHandle GraphVertexHandle::halfedge() const
{
    return GraphHalfedgeHandle(this->graph()->halfedge_handle(*this).idx(), this->graph());
}

bool GraphVertexHandle::is_boundary() const
{
    return this->graph()->is_boundary(*this);
}

/* GraphEdgeHandle make smart*/
GraphHalfedgeHandle GraphEdgeHandle::halfedge(unsigned i) const
{
    return GraphHalfedgeHandle(this->graph()->halfedge_handle(*this, i).idx(), this->graph());
}

bool GraphEdgeHandle::is_boundary() const
{
    return this->graph()->is_boundary(*this);
}

/* GraphHalfedgeHandle make smart*/
GraphHalfedgeHandle GraphHalfedgeHandle::next() const
{
    return GraphHalfedgeHandle(this->graph()->next_halfedge_handle(*this).idx(), this->graph());
}

GraphHalfedgeHandle GraphHalfedgeHandle::prev() const
{
    return GraphHalfedgeHandle(this->graph()->prev_halfedge_handle(*this).idx(), this->graph());
}

GraphHalfedgeHandle GraphHalfedgeHandle::opposite() const
{
    return GraphHalfedgeHandle(this->graph()->opposite_halfedge_handle(*this).idx(), this->graph());
}

GraphVertexHandle GraphHalfedgeHandle::from_vertex() const
{
    return GraphVertexHandle(this->graph()->from_vertex_handle(*this).idx(), this->graph());
}

GraphVertexHandle GraphHalfedgeHandle::to_vertex() const
{
    return GraphVertexHandle(this->graph()->to_vertex_handle(*this).idx(), this->graph());
}

GraphEdgeHandle GraphHalfedgeHandle::edge() const
{
    return GraphEdgeHandle(this->graph()->edge_handle(*this).idx(), this->graph());
}

GraphFaceHandle GraphHalfedgeHandle::face() const
{
    return GraphFaceHandle(this->graph()->face_handle(*this).idx(), this->graph());
}

bool GraphHalfedgeHandle::is_boundary() const
{
    return this->graph()->is_boundary(*this);
}

/**
 * @brief base class for all traits, usr traits should be derived from this class and override these traits
 */
struct DefaultTraits
{
    typedef std::array<double, 3> Point;
    typedef int VertexAttribute;
    typedef int HalfedgeAttribute;
    typedef int EdgeAttribute;
    typedef int FaceAttribute;
};

/**
 * @brief A graph describe the topological relationship and mesh items of the halfedge data struct
 * @tparam Traits element attributes
 */
template<class Traits = DefaultTraits>
class Graph : public GraphTopology
{
    public:
        typedef typename Traits::Point                               Point;
        typedef typename Traits::VertexAttribute                     VertexAttribute;
        typedef typename Traits::HalfedgeAttribute                   HalfedgeAttribute;
        typedef typename Traits::EdgeAttribute                       EdgeAttribute;
        typedef typename Traits::FaceAttribute                       FaceAttribute;

    public:
        /* constructor */
        Graph() : GraphTopology()
        {
        }

        /* deconstructor */
        ~Graph()
        {
        }
        
    public:
        /**
         * @brief add a vertex into graph and attach an attribute information
         * @return the new vertex handle
         */
        GraphVertexHandle add_vertex(const Point& p)
        {
            VertexHandle vh = new_vertex();
            point(vh) = p;
            return vertex(vh);
        }

        /* clear all elements and attributes */
        void clear()
        {
            GraphTopology::clear();
            _points.clear();
            _vertex_attr.clear();
            _halfedge_attr.clear();
            _edge_attr.clear();
            _face_attr.clear();
        }

    public:
        /* get the writable vertex point */
        Point& point(VertexHandle vh)
        {
            assert(vh.is_valid() && vh.idx() < n_vertices());
            if(vh.idx() >= _points.size())
                _points.resize(vh.idx() + 1);
            return _points[vh.idx()];
        }

        /* get the const vertex point */
        const Point& point(VertexHandle vh) const
        {
            assert(vh.is_valid() && vh.idx() < n_vertices() && vh.idx() < _points.size());
            return _points[vh.idx()];
        }

        /* get the writable vertex attribute */
        VertexAttribute& attribute(VertexHandle vh)
        {
            assert(vh.is_valid() && vh.idx() < n_vertices());
            if(vh.idx() >= _vertex_attr.size())
                _vertex_attr.resize(vh.idx() + 1);
            return _vertex_attr[vh.idx()];
        }

        /* get a const vertex attribute */
        const VertexAttribute& attribute(VertexHandle vh) const
        {
            assert(vh.is_valid() && vh.idx() < n_vertices() && vh.idx() < _vertex_attr.size());
            return _vertex_attr[vh.idx()];
        }

        /* get the writable halfedge attribute */
        HalfedgeAttribute& attribute(HalfedgeHandle heh)
        {
            assert(heh.is_valid() && heh.idx() < n_halfedges());
            if(heh.idx() >= _halfedge_attr.size())
                _halfedge_attr.resize(heh.idx() + 1);
            return _halfedge_attr[heh.idx()];
        }

        /* get a const halfedge attribute */
        const HalfedgeAttribute& attribute(HalfedgeHandle heh) const
        {
            assert(heh.is_valid() && heh.idx() < n_halfedges() && heh.idx() < _halfedge_attr.size());
            return _halfedge_attr[heh.idx()];
        }

        /* get the writable edge attribute */
        EdgeAttribute& attribute(EdgeHandle eh)
        {
            assert(eh.is_valid() && eh.idx() < n_edges());
            if(eh.idx() >= _edge_attr.size())
                _edge_attr.resize(eh.idx() + 1);
            return _edge_attr[eh.idx()];
        }

        /* get a const edge attribute */
        const EdgeAttribute& attribute(EdgeHandle eh) const
        {
            assert(eh.is_valid() && eh.idx() < n_edges() && eh.idx() < _edge_attr.size());
            return _edge_attr[eh.idx()];
        }

        /* get the writable face attribute */
        FaceAttribute& attribute(FaceHandle fh)
        {
            assert(fh.is_valid() && fh.idx() < n_edges());
            if(fh.idx() >= _face_attr.size())
                _face_attr.resize(fh.idx() + 1);
            return _edge_attr[fh.idx()];
        }

        /* get a const edge attribute */
        const FaceAttribute& attribute(FaceHandle fh) const
        {
            assert(fh.is_valid() && fh.idx() < n_edges() && fh.idx() < _face_attr.size());
            return _edge_attr[fh.idx()];
        }

    public:
        /* export into .obj format */
        void write_obj(const std::string& file) const
        {
            std::ofstream fout;
            fout.open(file.c_str(), std::ofstream::trunc);
            for(auto vit = this->vertices_begin(); vit != this->vertices_end(); ++vit)
            {
                const Point& p = this->point(*vit);
                fout << "v " << p[0] << " \t" << p[1] << " \t " << p[2] << '\n';
            }
            fout << std::endl;

            for(auto fit = this->faces_begin(); fit != this->faces_end(); ++fit)
            {
                fout << "f";
                for(auto fv = this->fv_begin(*fit); fv != this->fv_end(*fit); ++fv)
                {
                    fout << " " << fv->idx() + 1;
                }
                fout << std::endl;
            }
            fout.close();
        }

        /* read a .obj file */
        bool read_obj(const std::string& file)
        {
            std::fstream in(file.c_str(), std::ios_base::in);
            if(!in.is_open() || !in.good())
            {
                std::cerr << "error while opening file " << file << std::endl;
                return false;
            }
            this->clear();

            std::vector<VertexHandle> vhandles;
            std::vector<VertexHandle> fvhs;

            std::string line;
            std::string key;
            std::stringstream stream;
            while(in && !in.eof())
            {
                std::getline(in, line);
                if(in.bad())
                {
                    std::cerr << "error while reading obj line." << std::endl;
                    return false;
                }

                // trim
                size_t start = line.find_first_not_of(" \t\r\n");
                size_t end   = line.find_last_not_of(" \t\r\n");
                if((std::string::npos == start ) || (std::string::npos == end))
                    line = "";
                else
                    line = line.substr(start, end-start+1);

                if(line.empty() || line[0] == '#' || isspace(line[0]))
                    continue;

                stream.str(line);
                stream.clear();
                stream >> key;
                
                if(key == "v")
                {
                    Point p;
                    stream >> p[0]; stream >> p[1]; stream >> p[2];
                    if(stream.bad() || stream.fail())
                    {
                        std::cerr << "error while reading obj vertex." << std::endl;
                        return false;
                    }
                    vhandles.push_back(this->add_vertex(p));
                }
                else if(key == "f")
                {
                    int vid;
                    fvhs.clear();
                    while(stream >> vid)
                    {
                        if(vid > static_cast<int>(vhandles.size()) || vid == 0)
                        {
                            std::cerr << "error while reading obj face." << std::endl;
                            return false;
                        }
                        if(vid < 0)
                            fvhs.push_back(vhandles[static_cast<int>(vhandles.size()) + vid]);
                        else
                            fvhs.push_back(vhandles[vid-1]);

                        // we only read vertex in "v/vt/vn" format
                        stream.ignore(256, ' ');
                    }
                    if(stream.bad())
                    {
                        std::cerr << "error while reading obj face." << std::endl;
                        return false;
                    }
                    this->add_face(fvhs);
                }
                else if(key == "vt")
                {
                    // TODO
                }
                else if(key == "vn")
                {
                    // TODO
                }
            }

            in.close();
            return true;
        }

    protected:
        /* geometry information binding at vertices */
        std::vector<Point>  _points;

        /* attributes binding at vertices */
        std::vector<VertexAttribute>    _vertex_attr;

        /* attributes binding at halfedges  */
        std::vector<HalfedgeAttribute> _halfedge_attr;

        /* attributes binding at edges  */
        std::vector<EdgeAttribute>     _edge_attr;

        /* attributes binding at faces  */
        std::vector<FaceAttribute>     _face_attr;
};

}   // namespace halfedge

#endif // __halfedge_h__