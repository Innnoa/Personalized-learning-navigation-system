PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS courses (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    code TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL UNIQUE,
    description TEXT NOT NULL DEFAULT '',
    target_audience TEXT NOT NULL DEFAULT '',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS knowledge_points (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    chapter_no INTEGER NOT NULL,
    chapter_name TEXT NOT NULL,
    node_type TEXT NOT NULL DEFAULT 'concept',
    difficulty_level INTEGER NOT NULL DEFAULT 1 CHECK (difficulty_level BETWEEN 1 AND 5),
    estimated_minutes INTEGER NOT NULL DEFAULT 20 CHECK (estimated_minutes > 0),
    display_order INTEGER NOT NULL DEFAULT 0,
    is_core INTEGER NOT NULL DEFAULT 1 CHECK (is_core IN (0, 1)),
    description TEXT NOT NULL DEFAULT '',
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    UNIQUE (course_id, code),
    UNIQUE (course_id, display_order)
);

CREATE INDEX IF NOT EXISTS idx_knowledge_points_course_order
    ON knowledge_points(course_id, chapter_no, display_order);

CREATE TABLE IF NOT EXISTS knowledge_dependencies (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    prerequisite_point_id INTEGER NOT NULL,
    target_point_id INTEGER NOT NULL,
    relation_type TEXT NOT NULL DEFAULT 'prerequisite',
    description TEXT NOT NULL DEFAULT '',
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE,
    FOREIGN KEY (prerequisite_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE,
    FOREIGN KEY (target_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE,
    CHECK (prerequisite_point_id <> target_point_id),
    UNIQUE (course_id, prerequisite_point_id, target_point_id)
);

CREATE INDEX IF NOT EXISTS idx_knowledge_dependencies_course
    ON knowledge_dependencies(course_id);

CREATE TABLE IF NOT EXISTS learners (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    code TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    major TEXT NOT NULL DEFAULT '',
    grade_label TEXT NOT NULL DEFAULT '',
    target_course_id INTEGER NOT NULL,
    note TEXT NOT NULL DEFAULT '',
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (target_course_id) REFERENCES courses(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_learners_target_course
    ON learners(target_course_id);

CREATE TABLE IF NOT EXISTS learner_mastery (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    learner_id INTEGER NOT NULL,
    knowledge_point_id INTEGER NOT NULL,
    mastery_score REAL NOT NULL DEFAULT 0.0 CHECK (mastery_score BETWEEN 0.0 AND 1.0),
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (learner_id) REFERENCES learners(id) ON DELETE CASCADE,
    FOREIGN KEY (knowledge_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE,
    UNIQUE (learner_id, knowledge_point_id)
);

CREATE INDEX IF NOT EXISTS idx_learner_mastery_learner
    ON learner_mastery(learner_id);

CREATE TABLE IF NOT EXISTS learning_feedback_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    learner_id INTEGER NOT NULL,
    knowledge_point_id INTEGER NOT NULL,
    feedback_batch_id TEXT NOT NULL DEFAULT '',
    completion_status TEXT NOT NULL CHECK (
        completion_status IN ('completed', 'partial', 'blocked')
    ),
    self_rated_mastery REAL NOT NULL CHECK (self_rated_mastery BETWEEN 0.0 AND 1.0),
    previous_mastery REAL NOT NULL CHECK (previous_mastery BETWEEN 0.0 AND 1.0),
    updated_mastery REAL NOT NULL CHECK (updated_mastery BETWEEN 0.0 AND 1.0),
    rule_applied TEXT NOT NULL DEFAULT '',
    recorded_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (learner_id) REFERENCES learners(id) ON DELETE CASCADE,
    FOREIGN KEY (knowledge_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_learning_feedback_records_learner
    ON learning_feedback_records(learner_id, recorded_at DESC);

CREATE INDEX IF NOT EXISTS idx_learning_feedback_records_learner_batch
    ON learning_feedback_records(learner_id, feedback_batch_id, recorded_at DESC);

CREATE TABLE IF NOT EXISTS learner_detail_mastery (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    learner_id INTEGER NOT NULL,
    scope_code TEXT NOT NULL,
    node_code TEXT NOT NULL,
    mastery_score REAL NOT NULL DEFAULT 0.0 CHECK (mastery_score BETWEEN 0.0 AND 1.0),
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (learner_id) REFERENCES learners(id) ON DELETE CASCADE,
    UNIQUE (learner_id, scope_code, node_code)
);

CREATE INDEX IF NOT EXISTS idx_learner_detail_mastery_learner_scope
    ON learner_detail_mastery(learner_id, scope_code, updated_at DESC);

CREATE TABLE IF NOT EXISTS detail_learning_feedback_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    learner_id INTEGER NOT NULL,
    scope_code TEXT NOT NULL,
    node_code TEXT NOT NULL,
    feedback_batch_id TEXT NOT NULL DEFAULT '',
    completion_status TEXT NOT NULL CHECK (
        completion_status IN ('completed', 'partial', 'blocked')
    ),
    self_rated_mastery REAL NOT NULL CHECK (self_rated_mastery BETWEEN 0.0 AND 1.0),
    previous_mastery REAL NOT NULL CHECK (previous_mastery BETWEEN 0.0 AND 1.0),
    updated_mastery REAL NOT NULL CHECK (updated_mastery BETWEEN 0.0 AND 1.0),
    rule_applied TEXT NOT NULL DEFAULT '',
    recorded_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (learner_id) REFERENCES learners(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_detail_learning_feedback_records_learner_scope
    ON detail_learning_feedback_records(learner_id, scope_code, recorded_at DESC);

CREATE TABLE IF NOT EXISTS learning_resource_view_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    learner_id INTEGER NOT NULL,
    knowledge_point_id INTEGER NOT NULL,
    resource_title TEXT NOT NULL,
    resource_url TEXT NOT NULL,
    resource_type TEXT NOT NULL DEFAULT '',
    resource_source TEXT NOT NULL DEFAULT '',
    resource_layer TEXT NOT NULL DEFAULT '',
    recommended_phase TEXT NOT NULL DEFAULT '',
    source_context TEXT NOT NULL DEFAULT '',
    scope_code TEXT NOT NULL DEFAULT '',
    linked_reason_summary TEXT NOT NULL DEFAULT '',
    interaction_type TEXT NOT NULL DEFAULT 'opened' CHECK (
        interaction_type IN ('opened', 'viewed', 'completed', 'save_for_later')
    ),
    recorded_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (learner_id) REFERENCES learners(id) ON DELETE CASCADE,
    FOREIGN KEY (knowledge_point_id) REFERENCES knowledge_points(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_learning_resource_view_records_learner
    ON learning_resource_view_records(learner_id, recorded_at DESC);

CREATE INDEX IF NOT EXISTS idx_learning_resource_view_records_learner_point
    ON learning_resource_view_records(learner_id, knowledge_point_id, recorded_at DESC);
