--
-- A rather spiffy use of outer joins that gives the same information as
-- CIA's current web-basted stats catalog, of course much faster.
-- This puts stats_catalog on the left side of the joins, with a
-- second stats_catalog used to count children, and all the stats_metadata
-- and stats_counters instances necessary to extract the keys we're interested in.
-- Not amazingly efficient, but mind-bogglingly faster than doing this all
-- in Python on a Rack + libdb.
--
-- Yay SQL.
--
SELECT
    T.target_path            AS path,
    M_TITLE.value            AS title,
    C_TODAY.event_count      AS events_today,
    C_YESTERDAY.event_count  AS events_yesterday,
    C_FOREVER.event_count    AS events_forever,
    COUNT(CHILD.target_path) AS items
FROM stats_catalog T
    LEFT OUTER JOIN stats_catalog  CHILD       ON (CHILD.parent_path = T.target_path)
    LEFT OUTER JOIN stats_metadata M_TITLE     ON (T.target_path = M_TITLE.target_path     AND M_TITLE.name     = 'title')
    LEFT OUTER JOIN stats_counters C_TODAY     ON (T.target_path = C_TODAY.target_path     AND C_TODAY.name     = 'today')
    LEFT OUTER JOIN stats_counters C_YESTERDAY ON (T.target_path = C_YESTERDAY.target_path AND C_YESTERDAY.name = 'yesterday')
    LEFT OUTER JOIN stats_counters C_FOREVER   ON (T.target_path = C_FOREVER.target_path   AND C_FOREVER.name   = 'forever')
WHERE T.parent_path = 'author'
GROUP BY
    T.target_path,
    M_TITLE.value,
    C_TODAY.event_count,
    C_YESTERDAY.event_count,
    C_FOREVER.event_count;
