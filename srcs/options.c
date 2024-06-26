#include "options.h"

static void
set_option(t_option * option, char id, bool require_value) {
    option->id = id;
    option->active = false;
    option->require_value = require_value;
    option->value = NULL;
}

void
load_available_options(t_option * options) {
    set_option(options++, 'v', false);
    set_option(options++, 'h', false);
    set_option(options++, 's', true);
    set_option(options++, 't', true);
    set_option(options++, 'i', true);
    set_option(options++, 'c', true);
    set_option(options++, 'n', false);
    set_option(options++, 'q', false);
}

t_option *
get_option(t_option options[], char id) {
    for (int i = 0; i < OPTION_NUMBER; ++i)
        if (options[i].id == id)
            return (options + i);
    return (NULL);
}