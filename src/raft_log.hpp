﻿#pragma once
#include <raft/Storage.hpp>
#include <raft/Raft.hpp>
#include "log_unstable.hpp"
#include <raft/entrys.hpp>

namespace raft {
	class raft_log {
	public:
		raft_log(StoragePtr storage, Logger *logger, uint64_t maxNextEntsSize = noLimit);
		~raft_log();

		ErrorCode entries(vector<Entry> &out, uint64_t i, uint64_t maxsize = noLimit);
		ErrorCode slice(vector<Entry> &out, uint64_t lo, uint64_t hi, uint64_t maxSize = noLimit);
		template<class EntryContainer>
		bool maybeAppend(uint64_t index, uint64_t logTerm, uint64_t committed, const EntryContainer &ents, uint64_t &lastnewi) {
			return maybeAppend(index, logTerm, committed, (const IEntrySlice &)make_slice(ents), lastnewi);
		}
		bool maybeAppend(uint64_t index, uint64_t logTerm, uint64_t committed, const IEntrySlice &ents, uint64_t &lastnewi);
		bool matchTerm(uint64_t i, uint64_t term);
		bool maybeCommit(uint64_t maxIndex, uint64_t term);
		const EntryUnstableVec &unstableEntries();
		bool hasNextEnts();
		uint64_t lastTerm();
		vector<Entry> nextEnts();
		vector<Entry> allEntries();
		ErrorCode mustCheckOutOfBounds(uint64_t lo, uint64_t hi);
		ErrorCode term(uint64_t i, uint64_t &t);
		uint64_t appendSlice(const IEntrySlice &ents);
		template<class EntryContainer> uint64_t append(const EntryContainer &ents) {
			return appendSlice(make_slice(ents));
		}
		template<> uint64_t append<Entry>(const Entry &ent) {
			std::array<Entry, 1> s = { std::move(ent) };
			return appendSlice(make_slice(s));
		}
		template<class EntryContainer>
		uint64_t findConflict(const EntryContainer &ents) {
			return findConflict((const IEntrySlice &)make_slice(ents));
		}
		uint64_t findConflict(const IEntrySlice &ents);
		uint64_t firstIndex();
		uint64_t lastIndex();
		void restore(const Snapshot &s);
		string to_string();
		bool isUpToDate(uint64_t lasti, uint64_t term);
		void commitTo(uint64_t tocommit);
		void appliedTo(uint64_t i);
		void stableTo(uint64_t i, uint64_t t);
		void stableSnapTo(uint64_t i);
		uint64_t zeroTermOnErrCompacted(uint64_t t, ErrorCode err);
		ErrorCode snapshot(Snapshot **sn);

	public:
		StoragePtr storage;
		unstable unstable;

		uint64_t committed;
		uint64_t applied;

		Logger *logger;
		uint64_t maxNextEntsSize;
	};
}
