//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

#pragma once

#include <vector>
#include <set>
#include <cstring>
#include "sequence/seq.hpp"
#include "sequence/sequence.hpp"
#include "logger/logger.hpp"
#include "sequence/nucl.hpp"
#include "omni_utils.hpp"
#include "observable_graph.hpp"
#include "id_track_handler.hpp"
#include "order_and_law.hpp"

namespace omnigraph {

template<typename VertexIdT, typename EdgeIdT, class DataMasterT,
		typename VertexIt>
class AbstractEditableGraph: public ObservableGraph<VertexIdT, EdgeIdT, VertexIt> {
	typedef ObservableGraph<VertexIdT, EdgeIdT, VertexIt> base;
	//todo maybe rename template params themselves???
public:
	typedef VertexIdT VertexId;
	typedef EdgeIdT EdgeId;
	typedef DataMasterT DataMaster;
	typedef typename DataMaster::VertexData VertexData;
	typedef typename DataMaster::EdgeData EdgeData;
	typedef VertexIt VertexIterator;
  typedef typename base::edge_const_iterator edge_const_iterator;

private:
	//todo think of necessity to pull these typedefs through hierarchy
	DataMaster master_;

	mutable BaseIdTrackHandler<VertexIdT, EdgeIdT>* int_ids_;

	virtual VertexId CreateVertex(const VertexData &data) = 0;

	virtual void DestroyVertex(VertexId vertex) = 0;

	virtual VertexId HiddenAddVertex(const VertexData &data) = 0;

	virtual void HiddenDeleteVertex(VertexId v) = 0;

	virtual EdgeId HiddenAddEdge(VertexId v1, VertexId v2, const EdgeData &data) = 0;

	virtual EdgeId HiddenAddEdge(VertexId v1, VertexId v2,
			const EdgeData &data, restricted::IdDistributor * idDistributor) = 0;

	virtual void HiddenDeleteEdge(EdgeId edge) = 0;

	virtual vector<EdgeId> CorrectMergePath(const vector<EdgeId>& path) = 0;

	virtual vector<EdgeId> EdgesToDelete(const vector<EdgeId> &path) = 0;

	virtual vector<VertexId> VerticesToDelete(const vector<EdgeId> &path) = 0;

	void DeleteAllOutgoing(VertexId v) {
		TRACE("DeleteAllOutgoing " << OutgoingEdgeCount(v));
		while (OutgoingEdgeCount(v) > 0) {
			EdgeId edge = OutgoingEdges(v)[0];
			TRACE("DeleteOutgoing " << edge);
			DeleteEdge(edge);
			TRACE("DeleteOutgoing ok");
		}
	}

	void DeleteAllIncoming(VertexId v) {
		TRACE("DeleteAllIncoming " << IncomingEdgeCount(v));
		while (IncomingEdgeCount(v) > 0) {
			EdgeId edge = IncomingEdges(v)[0];
			TRACE("DeleteIncoming " << edge);
			DeleteEdge(edge);
			TRACE("DeleteIncoming ok");
		}TRACE("DeleteAllIncoming ok");
	}

	void FireDeletePath(const vector<EdgeId> &edgesToDelete,
			const vector<VertexId> &verticesToDelete) {
		for (auto it = edgesToDelete.begin(); it != edgesToDelete.end(); ++it)
			this->FireDeleteEdge(*it);
		for (auto it = verticesToDelete.begin(); it != verticesToDelete.end();
				++it)
			this->FireDeleteVertex(*it);
	}

	void HiddenDeletePath(const vector<EdgeId> &edgesToDelete,
			const vector<VertexId> &verticesToDelete) {
		for (auto it = edgesToDelete.begin(); it != edgesToDelete.end(); ++it)
			HiddenDeleteEdge(*it);
		for (auto it = verticesToDelete.begin(); it != verticesToDelete.end();
				++it)
			HiddenDeleteVertex(*it);
	}

public:
	typedef typename base::SmartVertexIt SmartVertexIt;
	typedef typename base::SmartEdgeIt SmartEdgeIt;

	AbstractEditableGraph(HandlerApplier<VertexId, EdgeId>* applier,
	const DataMaster& master) :
			base(applier), master_(master), int_ids_(NULL) {
	}

	virtual ~AbstractEditableGraph() {
		TRACE("~AbstractEditableGraph");
		//		doesn't work this way because call to virtual function is needed
		//		for (auto it = this->SmartVertexBegin(); !it.IsEnd(); ++it) {
		//			ForceDeleteVertex(*it);
		//		}
	}

	void set_int_ids(BaseIdTrackHandler<VertexIdT, EdgeIdT>* int_ids) const {
		VERIFY(!int_ids_ || !int_ids);
		int_ids_ = int_ids;
	}

	const BaseIdTrackHandler<VertexIdT, EdgeIdT> &int_ids() const {
		VERIFY(int_ids_);
		return *int_ids_;
	}
	BaseIdTrackHandler<VertexIdT, EdgeIdT>* ReturnIntIdPointer() const {return int_ids_;};

	int int_id(EdgeId edge) const {
		VERIFY(int_ids_);
		return int_ids_->ReturnIntId(edge);
	}

	int int_id(VertexId vertex) const {
		VERIFY(int_ids_);
		return int_ids_->ReturnIntId(vertex);
	}

	const DataMaster& master() const {
		return master_;
	}
protected:
	virtual bool AdditionalCompressCondition(VertexId v) const {
		return true;
	}
public:
	virtual const EdgeData& data(EdgeId edge) const = 0;

	virtual const VertexData& data(VertexId v) const = 0;

	virtual const vector<EdgeId> OutgoingEdges(VertexId v) const = 0;

  virtual edge_const_iterator out_begin(VertexId v) const = 0;

  virtual edge_const_iterator out_end(VertexId v) const = 0;
  
	virtual const vector<EdgeId> IncomingEdges(VertexId v) const = 0;

  virtual edge_const_iterator in_begin(VertexId v) const = 0;

  virtual edge_const_iterator in_end(VertexId v) const = 0;

	virtual size_t OutgoingEdgeCount(VertexId v) const = 0;

	virtual size_t IncomingEdgeCount(VertexId v) const = 0;

	virtual vector<EdgeId> GetEdgesBetween(VertexId v, VertexId u) const = 0;

	virtual VertexId EdgeStart(EdgeId edge) const = 0;

	virtual VertexId EdgeEnd(EdgeId edge) const = 0;

	virtual bool RelatedVertices(VertexId v1, VertexId v2) const = 0;

	bool CheckUniqueOutgoingEdge(VertexId v) const {
		return OutgoingEdgeCount(v) == 1;
	}

	EdgeId GetUniqueOutgoingEdge(VertexId v) const {
		VERIFY(CheckUniqueOutgoingEdge(v));
		return OutgoingEdges(v)[0];
	}

	bool CheckUniqueIncomingEdge(VertexId v) const {
		return IncomingEdgeCount(v) == 1;
	}

	EdgeId GetUniqueIncomingEdge(VertexId v) const {
		VERIFY(CheckUniqueIncomingEdge(v));
		return IncomingEdges(v)[0];
	}

	size_t length(const EdgeId edge) const {
		return master_.length(data(edge));
	}

	size_t length(const VertexId v) const {
		return master_.length(data(v));
	}

	VertexId AddVertex(const VertexData& data) {
		TRACE("Adding vertex");
		VertexId v = HiddenAddVertex(data);
		this->FireAddingVertex(v);
		this->FireAddVertex(v);
		TRACE("Vertex " << str(v) << " added");
		return v;
	}

	void DeleteVertex(VertexId v) {
		VERIFY(IsDeadEnd(v) && IsDeadStart(v));
		VERIFY(v != VertexId(NULL));
		TRACE("Deleting vertex " << str(v));
		this->FireDeleteVertex(v);
		HiddenDeleteVertex(v);
		TRACE("Vertex deleted");
	}

	void ForceDeleteVertex(VertexId v) {
		TRACE("Forcing deletion of vertex " << str(v));
		DeleteAllOutgoing(v);
		DeleteAllIncoming(v);
		DeleteVertex(v);
		TRACE("Vertex force-deleted");
	}

	EdgeId AddEdge(VertexId v1, VertexId v2, const EdgeData &data) {
		TRACE("Adding edge connecting " << str(v1) << " and " << str(v2));
		EdgeId e = HiddenAddEdge(v1, v2, data);
		this->FireAddingEdge(e);
		this->FireAddEdge(e);
		TRACE("Added edge " << str(e) << " connecting " << str(v1) << " and " << str(v2));
		return e;
	}

	//todo delete if not used
	bool HasEdge(VertexId v1, VertexId v2, const EdgeData &data) {
		vector<EdgeId> out = OutgoingEdges(v1);
		for (auto it = out.begin(); it != out.end(); ++it) {
			if ((EdgeEnd(*it) == v2) && (master_.equals(data(*it), data))) {
				return true;
			}
		}
		return false;
	}

	//todo delete if not used
	EdgeId GetEdge(VertexId v1, VertexId v2, const EdgeData &edgeData) {
		vector<EdgeId> out = OutgoingEdges(v1);
		for (auto it = out.begin(); it != out.end(); ++it) {
			if ((EdgeEnd(*it) == v2) && (master_.equals(data(*it), edgeData))) {
				return *it;
			}
		}
		return NULL;
	}

	void DeleteEdge(EdgeId e) {
		TRACE("Deleting edge " << str(e));
		this->FireDeleteEdge(e);
		HiddenDeleteEdge(e);
		TRACE("Edge deleted");
	}

	bool IsDeadEnd(VertexId v) const {
		return OutgoingEdgeCount(v) == 0;
	}

	bool IsDeadStart(VertexId v) const {
		return IncomingEdgeCount(v) == 0;
	}

	//todo refactor compression methods
	bool CanCompressVertex(VertexId v) const {
//		TRACE("Compress vertex check: ");
//		TRACE("Outgoing check: " << (OutgoingEdgeCount(v) == 1));
//		TRACE("Outgoing check: " << (CheckUniqueOutgoingEdge(v)));
//		TRACE("Incoming check: " << (IncomingEdgeCount(v) == 1));
//		TRACE("Incoming check: " << (CheckUniqueIncomingEdge(v) == 1));
//		cout << "oppa" << endl;
//		if((OutgoingEdgeCount(v) == 1) && (IncomingEdgeCount(v) == 1)) {
//			cout << "gopa " << endl;
//			cout << GetUniqueOutgoingEdge(v) << graph_.length(it2->first)endl;
//			cout << GetUniqueIncomingEdge(v) << endl;
//			TRACE("Loop check: " << (GetUniqueOutgoingEdge(v) != GetUniqueIncomingEdge(v)));
//			TRACE("Additional check: " << AdditionalCompressCondition(v));
//		}
		return OutgoingEdgeCount(v) == 1 && IncomingEdgeCount(v) == 1 /*one-in one-out*/
		&& GetUniqueOutgoingEdge(v) != GetUniqueIncomingEdge(v) /*not loop*/
		&& AdditionalCompressCondition(v);
	}

	void CompressVertex(VertexId v) {
		TRACE("Trying to compress vertex " << str(v));
		//VERIFY(CanCompressVertex(v));
		if (CanCompressVertex(v)) {
			UnsafeCompressVertex(v);
		} else {
			TRACE("Vertex " << str(v) << " can't be compressed");
		}
	}

	EdgeId UnsafeCompressVertex(VertexId v) {
		VERIFY(CanCompressVertex(v));
		TRACE("Compressing vertex " << str(v));
		vector<EdgeId> edges_to_merge;
		edges_to_merge.push_back(GetUniqueIncomingEdge(v));
		edges_to_merge.push_back(GetUniqueOutgoingEdge(v));
		return MergePath(edges_to_merge);
	}

	virtual std::string str(const EdgeId e) const {
//		return master_.str(data(edge));
		stringstream ss;
		if(int_ids_)
			ss << int_id(e) << " (" << length(e) << ")";
		else
			ss << e << " (" << length(e) << ")";
		return ss.str();
	}

	virtual std::string str(const VertexId v) const {
//		return master_.str(data(v));
		if(int_ids_)
			return ToString(int_id(v));
		else
			return ToString(v);
	}

	std::string detailed_str(const VertexId v) const {
		stringstream ss;
		ss << str(v) << ";";
		ss << "Incoming edges" << str(IncomingEdges(v)) << "; ";
		ss << "Outgoing edges" << str(OutgoingEdges(v)) << ";";
		return ss.str();
	}

	std::string detailed_str(const vector<EdgeId>& path) const {
		stringstream ss;
		ss << "Path: ";
		ss << "Vertex " << detailed_str(EdgeStart(path[0])) << " | ";
		for (auto it = path.begin(); it != path.end(); ++it) {
			EdgeId e = *it;
			ss << "Edge " << str(e) << " | ";
			ss << "Vertex " << detailed_str(EdgeEnd(e)) << " | ";
		}
		return ss.str();
	}

	template<class Container>
	std::string str(const Container& container) const {
		return str(container.begin(), container.end());
	}

	template<class It>
	std::string str(It begin, It end) const {
		stringstream ss;
		string delim = "";
		for (auto it = begin; it != end; ++it) {
			ss << delim << str(*it);
			delim = ", ";
		}
		return ss.str();
	}

	EdgeId MergePath(const vector<EdgeId>& path) {
		VERIFY(!path.empty());
		for (size_t i = 0; i < path.size(); i++)
			for (size_t j = i + 1; j < path.size(); j++) {
				VERIFY(path[i] != path[j]);
			}
		if (path.size() == 1) {
			TRACE(
					"Path of single edge " << str(*(path.begin())) << ". Nothing to merge.");
		}TRACE("Merging path of edges " << str(path));

		//		cerr << "Merging " << PrintDetailedPath(pObservableGraph<VertexIdT, EdgeIdT, VertexIt>ath) << endl;
		//		cerr << "Conjugate " << PrintConjugatePath(path) << endl;
		vector<EdgeId> corrected_path = CorrectMergePath(path);
		TRACE("Corrected path: " << str(corrected_path));
		VertexId v1 = EdgeStart(corrected_path[0]);
		VertexId v2 = EdgeEnd(corrected_path[corrected_path.size() - 1]);
		vector<const EdgeData*> to_merge;
		for (auto it = corrected_path.begin(); it != corrected_path.end();
				++it) {
			to_merge.push_back(&(data(*it)));
		}
		EdgeId new_edge = HiddenAddEdge(v1, v2, master_.MergeData(to_merge));
		this->FireAddingEdge(new_edge);
		this->FireMerge(corrected_path, new_edge);

		//		cerr << "Corrected " << PrintDetailedPath(corrected_path) << endl;
		//		cerr << "Corrected conjugate " << PrintConjugatePath(corrected_path) << endl;
		vector<EdgeId> edges_to_delete = EdgesToDelete(corrected_path);
		//		cerr << "To delete " << PrintEdges(edges_to_delete) << endl;
		vector<VertexId> vertices_to_delete = VerticesToDelete(corrected_path);
		//		cerr << "To delete " << PrintVertices(vertices_to_delete) << endl;

		this->FireDeletePath(edges_to_delete, vertices_to_delete);
		this->FireAddEdge(new_edge);
		HiddenDeletePath(edges_to_delete, vertices_to_delete);
		TRACE(
				"Path merged. Corrected path merged into " << str(new_edge));
		return new_edge;
	}

	pair<EdgeId, EdgeId> SplitEdge(EdgeId edge, size_t position) {
		TRACE("Splitting edge " << str(edge) << " in position " << position);
		VERIFY_MSG(position > 0 && position < length(edge)
				, "Edge length is " << length(edge) << " but split pos was " << position);
		TRACE("Splitting edge " << str(edge));
		pair<VertexData, pair<EdgeData, EdgeData>> newData = master_.SplitData(
				data(edge), position);
		VertexId splitVertex = HiddenAddVertex(newData.first);
		this->FireAddingVertex(splitVertex);
		EdgeId new_edge1 = HiddenAddEdge(EdgeStart(edge), splitVertex,
				newData.second.first);
		this->FireAddingEdge(new_edge1);
		EdgeId new_edge2 = HiddenAddEdge(splitVertex, EdgeEnd(edge),
				newData.second.second);
		this->FireAddingEdge(new_edge2);
		this->FireSplit(edge, new_edge1, new_edge2);
		this->FireDeleteEdge(edge);
		this->FireAddVertex(splitVertex);
		this->FireAddEdge(new_edge1);

		this->FireAddEdge(new_edge2);
		HiddenDeleteEdge(edge);
		TRACE(
				"Edge split into edges " << str(new_edge1) << " and " << str(new_edge2));
		return make_pair(new_edge1, new_edge2);
	}

	EdgeId GlueEdges(EdgeId edge1, EdgeId edge2) {
		TRACE(
				"Gluing edges " << str(edge1) << " and " << str(edge2));
		EdgeId new_edge = HiddenAddEdge(EdgeStart(edge2), EdgeEnd(edge2),
				master_.GlueData(data(edge1), data(edge2)));
		this->FireAddingEdge(new_edge);
		this->FireGlue(new_edge, edge1, edge2);
		this->FireDeleteEdge(edge1);
		this->FireDeleteEdge(edge2);
		this->FireAddEdge(new_edge);
		VertexId start = EdgeStart(edge1);
		VertexId end = EdgeEnd(edge1);
		HiddenDeleteEdge(edge1);
		HiddenDeleteEdge(edge2);
		if (IsDeadStart(start) && IsDeadEnd(start)) {
			DeleteVertex(start);
		}
		if (IsDeadStart(end) && IsDeadEnd(end)) {
			DeleteVertex(end);
		}TRACE(
				"Edges glued into " << str(new_edge));
		return new_edge;
	}

private:
	DECL_LOGGER("AbstractEditableGraph")
};

}

