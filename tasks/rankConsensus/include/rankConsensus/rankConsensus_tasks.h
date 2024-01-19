//
// Created by lukemartinlogan on 8/11/23.
//

#ifndef HRUN_TASKS_TASK_TEMPL_INCLUDE_rankConsensus_rankConsensus_TASKS_H_
#define HRUN_TASKS_TASK_TEMPL_INCLUDE_rankConsensus_rankConsensus_TASKS_H_

#include "hrun/api/hrun_client.h"
#include "hrun/task_registry/task_lib.h"
#include "hrun_admin/hrun_admin.h"
#include "hrun/queue_manager/queue_manager_client.h"
#include "proc_queue/proc_queue.h"

namespace hrun::rankConsensus {

#include "rankConsensus_methods.h"
#include "hrun/hrun_namespace.h"
using hrun::proc_queue::TypedPushTask;
using hrun::proc_queue::PushTask;

/**
 * A task to create rankConsensus
 * */
using hrun::Admin::CreateTaskStateTask;
struct ConstructTask : public CreateTaskStateTask {
  /** SHM default constructor */
  HSHM_ALWAYS_INLINE explicit
  ConstructTask(hipc::Allocator *alloc)
  : CreateTaskStateTask(alloc) {}

  /** Emplace constructor */
  HSHM_ALWAYS_INLINE explicit
  ConstructTask(hipc::Allocator *alloc,
                const TaskNode &task_node,
                const DomainId &domain_id,
                const std::string &state_name,
                const TaskStateId &id,
                const std::vector<PriorityInfo> &queue_info)
      : CreateTaskStateTask(alloc, task_node, domain_id, state_name,
                            "rankConsensus", id, queue_info) {
    // Custom params
  }

  HSHM_ALWAYS_INLINE
  ~ConstructTask() {
    // Custom params
  }
};

/** A task to destroy rankConsensus */
using hrun::Admin::DestroyTaskStateTask;
struct DestructTask : public DestroyTaskStateTask {
  /** SHM default constructor */
  HSHM_ALWAYS_INLINE explicit
  DestructTask(hipc::Allocator *alloc)
  : DestroyTaskStateTask(alloc) {}

  /** Emplace constructor */
  HSHM_ALWAYS_INLINE explicit
  DestructTask(hipc::Allocator *alloc,
               const TaskNode &task_node,
               const DomainId &domain_id,
               TaskStateId &state_id)
  : DestroyTaskStateTask(alloc, task_node, domain_id, state_id) {}

  /** Create group */
  HSHM_ALWAYS_INLINE
  u32 GetGroup(hshm::charbuf &group) {
    return TASK_UNORDERED;
  }
};

/**
 * A custom task in rankConsensus
 * */
struct GetRankTask : public Task, TaskFlags<TF_SRL_SYM> {
  OUT uint rank_;
  /** SHM default constructor */
  HSHM_ALWAYS_INLINE explicit
  GetRankTask(hipc::Allocator *alloc) : Task(alloc) {}

  /** Emplace constructor */
  HSHM_ALWAYS_INLINE explicit
  GetRankTask(hipc::Allocator *alloc,
             const TaskNode &task_node,
             const DomainId &domain_id,
             const TaskStateId &state_id) : Task(alloc) {
    // Initialize task
    task_node_ = task_node;
    lane_hash_ = 0;
    prio_ = TaskPrio::kLowLatency;
    task_state_ = state_id;
    method_ = Method::kGetRank;
    task_flags_.SetBits(0);
    domain_id_ = domain_id;

    std::cout << "task constructor" << std::endl;

    // Custom params
  }

  /** (De)serialize message call */
  template<typename Ar>
  void SerializeStart(Ar &ar) {
    task_serialize<Ar>(ar);
  }

  /** (De)serialize message return */
  template<typename Ar>
  void SerializeEnd(u32 replica, Ar &ar) {
    std::cout << "end " << task->rank_ << std::endl;
    ar(rank_);
  }

  /** Duplicate message */
  void Dup(hipc::Allocator *alloc, GetRankTask &other) {
    task_dup(other);
  }

  /** Process duplicate message output */
  void DupEnd(u32 replica, GetRankTask &dup_task) {
  }

  /** Begin replication */
  void ReplicateStart(u32 count) {
  }

  /** Finalize replication */
  void ReplicateEnd() {}

  /** Create group */
  HSHM_ALWAYS_INLINE
  u32 GetGroup(hshm::charbuf &group) {
    return TASK_UNORDERED;
  }
};

}  // namespace hrun::rankConsensus

#endif  // HRUN_TASKS_TASK_TEMPL_INCLUDE_rankConsensus_rankConsensus_TASKS_H_
