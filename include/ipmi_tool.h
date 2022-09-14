#ifndef QUERRYNCLUDE_H_
#define QUERRYNCLUDE_H_

#include <stdbool.h>

typedef enum log_style_e { // Log style for demeter log output.
    FANCY=0,
    SIMPLE=1,
    SYSTEM=2,
} log_style_t;

typedef enum dem_log_level_e { // Log level for demeter log output.
    DEBUG=0,
    INFO=1,
    WARNING=2,
    ERROR=3,
    FATAL=4,
} dem_log_level_t;

typedef struct linked_list_s { // Generic linked list.
    void *data; // Needs to be casted to the appropriate type.
    struct linked_list_s *next;
} linked_list_t;

typedef struct parsed_sel_s {
    char *unparsed_sel; // Raw sel line.
    char *sel_time_str; // Time & date of sel in readable format.
    char *sel_msg_type; // Type of sel message.
    char *sel_msg; // Actual sel message.
    bool asserted; // True if sel message is "asserted".
} parsed_sel_t;

typedef struct demeter_conf_s { // Demeter configuration.
    uint verbose_lv; // Verbosity level if log level is DEBUG.
    dem_log_level_t log_level; // Log level for demeter log output.
    log_style_t log_style;
    char *log_file_path; // Path to demeter log file.
    char *slurm_log_path; // Path to file in which demeter will parse log from. (to be implemented)
    dem_log_level_t slurm_log_level; // Log level for slurm log parsing.
    dem_log_level_t sys_log_level; // Log level for system log parsing.
} demeter_conf_t;

typedef struct job_id_info_s {
    uint job_id; // Id from current job when setup propperly.
    uint uid; // User id from current job when setup propperly.
    uint step_id; // Id from current job step when setup propperly.
    time_t start_time; // "Start time for job", actually time of
    //execution of the acct_gather_profile_p_node_step_start function in the plugin.
    // Used to verify that logs gathered are logs that are timestamped in job runtime.
} job_id_info_t;

typedef struct cgroup_data_s { // Cgroup gathered data for each job step || job.
    uint mem_max_usage_bytes; // Maximum memory usage in bytes.
    uint oom_kill_disable;
    uint under_oom;
    uint oom_kill; 
    char *cpuset_cpus;
    char *cpuset_effective_cpus;
} cgroup_data_t;

#endif /* !QUERRYNCLUDE_H_ */
