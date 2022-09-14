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

int len_untill(char *str, char c)
{
    int i = 0;

    while (str[i] != c && str[i] != '\0')
        i++;
    return (i);
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
            printf(((parsed_sel_t *)gathered_sel->data)->asserted? "Asserted\n" : "Deasserted\n");
            printf("_____________________________________________\n");
		}
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