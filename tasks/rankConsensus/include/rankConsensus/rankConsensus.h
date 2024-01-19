/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HRUN_rankConsensus_H_
#define HRUN_rankConsensus_H_

#include "rankConsensus_tasks.h"

namespace hrun::rankConsensus {

/** Create rankConsensus requests */
class Client : public TaskLibClient {

 public:
  /** Default constructor */
  Client() = default;

  /** Destructor */
  ~Client() = default;

  /** Async create a task state */
  HSHM_ALWAYS_INLINE
  LPointer<ConstructTask> AsyncCreate(const TaskNode &task_node,
                                      const DomainId &domain_id,
                                      const std::string &state_name) {
    id_ = TaskStateId::GetNull();
    QueueManagerInfo &qm = HRUN_CLIENT->server_config_.queue_manager_;
    std::vector<PriorityInfo> queue_info;
    return HRUN_ADMIN->AsyncCreateTaskState<ConstructTask>(
        task_node, domain_id, state_name, id_, queue_info);
  }
  HRUN_TASK_NODE_ROOT(AsyncCreate)
  template<typename ...Args>
  HSHM_ALWAYS_INLINE
  void CreateRoot(Args&& ...args) {
    LPointer<ConstructTask> task =
        AsyncCreateRoot(std::forward<Args>(args)...);
    task->Wait();
    Init(id_, HRUN_ADMIN->queue_id_);
    HRUN_CLIENT->DelTask(task);
  }

  /** Destroy task state + queue */
  HSHM_ALWAYS_INLINE
  void DestroyRoot(const DomainId &domain_id) {
    HRUN_ADMIN->DestroyTaskStateRoot(domain_id, id_);
  }

  /** Call a custom method */
  HSHM_ALWAYS_INLINE
  void AsyncGetRankConstruct(GetRankTask *task,
                            const TaskNode &task_node,
                            const DomainId &domain_id) {
    HRUN_CLIENT->ConstructTask<GetRankTask>(
        task, task_node, domain_id, id_);
  }
  HSHM_ALWAYS_INLINE
  uint GetRankRoot(const DomainId &domain_id) {
    std::cout << "root " << std::endl;

    LPointer<hrunpq::TypedPushTask<GetRankTask>> get_task = AsyncGetRankRoot(domain_id);
    std::cout << "set up " << std::endl;
//    get_task.ptr_->Wa it();
    get_task->Wait();
    std::cout << "done wait " << std::endl;

    GetRankTask *task = get_task->get();
    uint choosen_rank = task->rank_;
    std::cout << "root rank " << choosen_rank << std::endl;
    HRUN_CLIENT->DelTask(get_task);
    return choosen_rank;
  }
  HRUN_TASK_NODE_PUSH_ROOT(GetRank);
};

}  // namespace hrun

#endif  // HRUN_rankConsensus_H_
