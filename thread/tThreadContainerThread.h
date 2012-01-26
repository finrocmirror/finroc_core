

#ifndef core__thread__tThreadContainerThread_h__
#define core__thread__tThreadContainerThread_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/tFrameworkElement.h"
#include "core/tCoreFlags.h"
#include "core/port/tEdgeAggregator.h"
#include "core/thread/tCoreLoopThreadBase.h"
#include "core/tRuntimeListener.h"
#include "core/port/tPort.h"

namespace finroc
{
namespace core
{
class tThreadContainer;
class tPeriodicFrameworkElementTask;
class tAbstractPort;

/*! ThreadContainer thread class */
class tThreadContainerThread : public tCoreLoopThreadBase, public tRuntimeListener
{
private:

  /*! Thread container that thread belongs to */
  tFrameworkElement* thread_container;

  /*! true, when thread needs to make a new schedule before next run */
  volatile bool reschedule;

  /*! simple schedule: Tasks will be executed in specified order */
  util::tSimpleList<tPeriodicFrameworkElementTask*> schedule;

  /*! temporary list of tasks that need to be scheduled */
  util::tSimpleList<tPeriodicFrameworkElementTask*> tasks;

  /*! temporary list of tasks that need to be scheduled - which are not sensor tasks */
  util::tSimpleList<tPeriodicFrameworkElementTask*> non_sensor_tasks;

  /*! temporary variable for scheduling algorithm: trace we're currently following */
  util::tSimpleList<tEdgeAggregator*> trace;

  /*! temporary variable: trace back */
  util::tSimpleList<tPeriodicFrameworkElementTask*> trace_back;

  /*! tree filter to search for tasks */
  tFrameworkElementTreeFilter filter;

  /*! temp buffer */
  util::tStringBuilder tmp;

  /*! Port to publish time spent in last call to MainLoopCallback() */
  tPort<long> last_cycle_execution_time;

private:

  /*!
   * Trace outgoing connection
   *
   * \param task Task we're tracing from
   * \param outgoing edge aggregator with outgoing connections to follow
   */
  void TraceOutgoing(tPeriodicFrameworkElementTask* task, tEdgeAggregator* outgoing);

public:

  tThreadContainerThread(tFrameworkElement* thread_container_, int64 default_cycle_time, bool warn_on_cycle_time_exceed, tPort<long> last_cycle_execution_time);

  /*!
   * \param fe Framework element
   * \return Is framework element an interface?
   */
  inline bool IsInterface(tFrameworkElement* fe)
  {
    return fe->GetFlag(tCoreFlags::cEDGE_AGGREGATOR | tEdgeAggregator::cIS_INTERFACE);
  }

  virtual void MainLoopCallback();

  virtual void Run();

  virtual void RuntimeChange(int8 change_type, tFrameworkElement* element);

  virtual void RuntimeEdgeChange(int8 change_type, tAbstractPort* source, tAbstractPort* target);

  virtual void StopThread();

  void TreeFilterCallback(tFrameworkElement* fe, bool unused);

};

} // namespace finroc
} // namespace core

#endif // core__thread__tThreadContainerThread_h__
