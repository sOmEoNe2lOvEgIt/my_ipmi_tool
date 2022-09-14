#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "ipmi_tool.h"

bool is_log_empty(char *log)
{
    if (log == NULL)
        return (true);
    if (strlen(log) == 0 || strcmp(log, "\n") == 0) {
        free(log);
        return (true);
    }
    return (false);
}

parsed_sel_t *init_parsed_sel(void)
{
    parsed_sel_t *parsed_sel = malloc(sizeof(parsed_sel_t));

    parsed_sel->unparsed_sel = NULL;
    parsed_sel->sel_time_str = NULL;
    parsed_sel->sel_msg = NULL;
    parsed_sel->sel_msg_type = NULL;
    parsed_sel->asserted = false;
    return (parsed_sel);
}

linked_list_t *add_to_list(linked_list_t *list, void *data)
{
    linked_list_t *new_link = (linked_list_t *)malloc(sizeof(linked_list_t));

    if (new_link == NULL)
        return (NULL);
    if (data == NULL)
        new_link->data = NULL;
    else
        new_link->data = data;
    if (list == NULL)
        new_link->next = NULL;
    else
        new_link->next = list;
    return (new_link);
}

int handle_sel_assert(parsed_sel_t *curr_sel)
{
    int i = 0;
    int j = 0;
    int len = 0;

    for (; j < 4; j++)
        for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0')
        return (1);
    i += 2;
    for (; curr_sel->unparsed_sel[i] != '\0'; i++, len++);
    if (strstr(&curr_sel->unparsed_sel[i - len], "Asserted") != NULL)
        curr_sel->asserted = true;
    else
        curr_sel->asserted = false;
    return (0);
}

int handle_sel_time(parsed_sel_t *curr_sel, time_t start_time)
{
    int i = 0;
    struct tm *sel_time = malloc(sizeof(struct tm));
    char time_str[80];

    for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0' && curr_sel->unparsed_sel[i + 2] == '\0')
        return (1);
    i += 2;
    sel_time->tm_mday = atoi(&curr_sel->unparsed_sel[i]);
    for (; curr_sel->unparsed_sel[i] != '/' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0' && curr_sel->unparsed_sel[i + 1] == '\0')
        return (1);
    i++;
    sel_time->tm_mon = atoi(&curr_sel->unparsed_sel[i]);
    for (; curr_sel->unparsed_sel[i] != '/' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0' && curr_sel->unparsed_sel[i + 1] == '\0')
        return (1);
    i++;
    sel_time->tm_year = atoi(&curr_sel->unparsed_sel[i]);
    for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0' && curr_sel->unparsed_sel[i + 2] == '\0')
        return (1);
    i += 2;
    sel_time->tm_hour = atoi(&curr_sel->unparsed_sel[i]);
    for (; curr_sel->unparsed_sel[i] != ':' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0' && curr_sel->unparsed_sel[i + 1] == '\0')
        return (1);
    i++;
    sel_time->tm_min = atoi(&curr_sel->unparsed_sel[i]);
    for (; curr_sel->unparsed_sel[i] != ':' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0' && curr_sel->unparsed_sel[i + 1] == '\0')
        return (1);
    i++;
    sel_time->tm_sec = atoi(&curr_sel->unparsed_sel[i]);
    sprintf(time_str, "[%d-%02d-%02dT%02d:%02d:%02d]", sel_time->tm_year, sel_time->tm_mon, sel_time->tm_mday, sel_time->tm_hour, sel_time->tm_min, sel_time->tm_sec);
    curr_sel->sel_time_str = strdup(time_str);
    free(sel_time);
    return (0);
}

int handle_sel_type(parsed_sel_t *curr_sel)
{
    int i = 0;
    int j = 0;
    int len = 0;

    for (; j < 2; j++)
        for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0')
        return (1);
    i++;
    for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++, len++);
    len --;
    curr_sel->sel_msg_type = strndup(&curr_sel->unparsed_sel[i - len], len);
    return (0);
}

int handle_sel_msg(parsed_sel_t *curr_sel)
{
    int i = 0;
    int j = 0;
    int len = 0;

    for (; j < 3; j++)
        for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++);
    if (curr_sel->unparsed_sel[i] == '\0')
        return (1);
    i++;
    for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++, len++);
    if (curr_sel->unparsed_sel[i] == '\0')
        return (1);
    i++;
    for (; curr_sel->unparsed_sel[i] != '|' && curr_sel->unparsed_sel[i] != '\0'; i++, len++);
    if (curr_sel->unparsed_sel[i] == '\0')
        return (1);
    len --;
    curr_sel->sel_msg = strndup(&curr_sel->unparsed_sel[i - len], len);
    curr_sel->sel_msg[len] = '\0';
    return (0);
}

linked_list_t *gather_sel_logs
(job_id_info_t *job_info, linked_list_t *sel_list)
{
    FILE *log_file = NULL;
    parsed_sel_t *curr_log = NULL;
    char *buffer = NULL;
    size_t len = 1000;

    if ((log_file = popen("ipmitool -U admin -P password sel list", "r")) == NULL)
        return (NULL);
    sel_list = add_to_list(sel_list, init_parsed_sel());
    while (getline(&buffer, &len, log_file) != -1) {
        curr_log = (parsed_sel_t *)sel_list->data;
        curr_log->unparsed_sel = strdup(buffer);
        if (!handle_sel_time(curr_log, job_info->start_time))
            continue;
        if (!handle_sel_type(curr_log))
            continue;
        if (!handle_sel_msg(curr_log))
            continue;
        if (!handle_sel_assert(curr_log))
            continue;
        sel_list = add_to_list(sel_list, init_parsed_sel());
    }
    pclose(log_file);
    return (sel_list);
}

linked_list_t *gather_sel(job_id_info_t *job_info)
{
    linked_list_t *sel_list = NULL;

    sel_list = gather_sel_logs(job_info, sel_list);
    return (sel_list);
}

void log_parsed_sel(linked_list_t *gathered_sel)
{
	if (gathered_sel == NULL) {
		printf("no logs gathered");
		return;
	}
	while (gathered_sel->next != NULL && is_log_empty(((parsed_sel_t *)gathered_sel->data)->unparsed_sel))
		gathered_sel = gathered_sel->next;
	while (gathered_sel != NULL) {
    	if (gathered_sel != NULL && !is_log_empty(((parsed_sel_t *)gathered_sel->data)->unparsed_sel)) {
			printf(((parsed_sel_t *)gathered_sel->data)->unparsed_sel);
            printf("%s\n", ((parsed_sel_t *)gathered_sel->data)->sel_time_str);
            printf("%s\n", ((parsed_sel_t *)gathered_sel->data)->sel_msg_type);
            printf("%s\n", ((parsed_sel_t *)gathered_sel->data)->sel_msg);
            printf(((parsed_sel_t *)gathered_sel->data)->asserted? "asserted\n" : "deasserted\n");
		} else
			printf("no worth logs gathered");
		gathered_sel = gathered_sel->next;
	}
}

int main (int ac, char **av)
{
    job_id_info_t *job_info = malloc(sizeof(job_id_info_t));
    linked_list_t *sel_list = NULL;
    time_t start_time = 0;

    job_info->start_time = start_time;
    sel_list = gather_sel(job_info);
    if (sel_list == NULL)
        return (1);
    log_parsed_sel(sel_list);
    return (0);
}