PRAGMA foreign_keys = ON;

INSERT INTO courses (code, name, description, target_audience)
VALUES (
    'data-structures',
    '数据结构',
    '本科毕设演示课程，覆盖数据结构课程的核心知识点依赖主线。',
    '计算机类本科生'
);

INSERT INTO knowledge_points (
    course_id,
    code,
    name,
    chapter_no,
    chapter_name,
    node_type,
    difficulty_level,
    estimated_minutes,
    display_order,
    is_core,
    description
)
VALUES
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'ds-intro', '数据结构基本概念', 1, '绪论', 'foundation', 1, 20, 10, 1, '理解数据、数据元素、逻辑结构、存储结构与抽象数据类型。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'algorithm-analysis', '算法复杂度分析', 1, '绪论', 'algorithm', 2, 30, 20, 1, '掌握时间复杂度与空间复杂度的基本分析方法。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'linear-list', '线性表', 2, '线性表', 'structure', 2, 35, 30, 1, '建立线性表抽象数据类型与基本操作的整体认识。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'sequence-list', '顺序表', 2, '线性表', 'implementation', 2, 35, 40, 1, '理解顺序存储结构、插入删除和容量约束。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'linked-list', '链表', 2, '线性表', 'implementation', 3, 40, 50, 1, '理解单链表及插入删除、头插尾插等实现方式。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'stack', '栈', 3, '栈和队列', 'structure', 2, 30, 60, 1, '掌握后进先出特性及其典型应用。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'queue', '队列', 3, '栈和队列', 'structure', 2, 30, 70, 1, '掌握先进先出特性、循环队列与链队列的基本思想。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'string', '串', 4, '串、数组和广义表', 'structure', 2, 25, 80, 1, '理解串的定义、存储和基本操作。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'kmp', 'KMP 模式匹配', 4, '串、数组和广义表', 'algorithm', 4, 40, 90, 1, '掌握模式匹配问题与 KMP 算法的核心思想。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'tree-basic', '树与二叉树基础', 5, '树和二叉树', 'structure', 3, 40, 100, 1, '理解树、二叉树的定义、性质和存储结构。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'binary-tree-traversal', '二叉树遍历', 5, '树和二叉树', 'algorithm', 3, 35, 110, 1, '掌握先序、中序、后序和层序遍历的策略。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'huffman-tree', '哈夫曼树与编码', 5, '树和二叉树', 'application', 4, 35, 120, 1, '理解最优二叉树构造与编码应用。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'graph-basic', '图的存储与遍历', 6, '图', 'structure', 4, 45, 130, 1, '理解图的邻接矩阵、邻接表以及 DFS/BFS。'),
    ((SELECT id FROM courses WHERE code = 'data-structures'), 'topological-sort', '拓扑排序与关键路径', 6, '图', 'algorithm', 5, 45, 140, 1, '理解有向无环图、拓扑排序和关键路径分析。');

INSERT INTO knowledge_dependencies (
    course_id,
    prerequisite_point_id,
    target_point_id,
    relation_type,
    description
)
VALUES
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'ds-intro' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'algorithm-analysis' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '复杂度分析建立在基础数据结构与算法概念之上。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'ds-intro' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'linear-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '线性表是进入具体数据结构实现前的第一类基本线性结构。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'linear-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'sequence-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '顺序表是线性表的顺序存储实现。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'linear-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'linked-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '链表是线性表的链式存储实现。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'linear-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'stack' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '栈可以视为受限线性表，先掌握线性表更容易理解其抽象。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'linear-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'queue' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '队列可以视为另一类受限线性表。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'ds-intro' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'string' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '串的概念建立在基础数据结构抽象之上。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'string' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'kmp' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '先理解串的表示与匹配问题，才能进入 KMP。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'algorithm-analysis' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'kmp' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        'KMP 需要结合复杂度分析理解其相对朴素匹配的优化价值。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'ds-intro' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'tree-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '树和二叉树属于更复杂的非线性结构，需要先具备基础结构概念。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'tree-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'binary-tree-traversal' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '遍历策略建立在二叉树结构定义与存储的理解之上。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'tree-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'huffman-tree' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '哈夫曼树是二叉树的重要应用场景。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'ds-intro' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'graph-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '图的定义与存储同样建立在基础数据结构概念之上。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'graph-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'topological-sort' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        '拓扑排序与关键路径需要先理解图的表示与遍历。'
    ),
    (
        (SELECT id FROM courses WHERE code = 'data-structures'),
        (SELECT id FROM knowledge_points WHERE code = 'queue' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        (SELECT id FROM knowledge_points WHERE code = 'topological-sort' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'prerequisite',
        'Kahn 风格的拓扑排序会用到队列思想。'
    );

INSERT INTO learners (
    code,
    name,
    major,
    grade_label,
    target_course_id,
    note
)
VALUES (
    'demo-learner',
    '演示学习者',
    '计算机科学与技术',
    '大三',
    (SELECT id FROM courses WHERE code = 'data-structures'),
    '默认演示学习者，用于展示学习者画像、反馈记录与动态路径规划。'
);

INSERT INTO learner_mastery (
    learner_id,
    knowledge_point_id,
    mastery_score
)
VALUES
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'ds-intro' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.90
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'algorithm-analysis' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.70
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'linear-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.82
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'sequence-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.58
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'linked-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.46
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'stack' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.60
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'queue' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.35
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'string' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.40
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'kmp' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.12
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'tree-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.38
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'binary-tree-traversal' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.18
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'huffman-tree' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.05
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'graph-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.15
    ),
    (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'topological-sort' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        0.00
    );

INSERT OR IGNORE INTO users (
    username,
    password_hash,
    display_name,
    status
)
VALUES
    ('student_demo', 'demo123', '演示学生账号', 'active'),
    ('teacher_demo', 'demo123', '演示教师账号', 'active'),
    ('admin_demo', 'demo123', '演示管理员账号', 'active');

INSERT OR IGNORE INTO user_roles (
    user_id,
    role_code
)
VALUES
    ((SELECT id FROM users WHERE username = 'student_demo'), 'student'),
    ((SELECT id FROM users WHERE username = 'teacher_demo'), 'teacher'),
    ((SELECT id FROM users WHERE username = 'admin_demo'), 'admin');

INSERT OR IGNORE INTO user_learner_links (
    user_id,
    learner_id
)
VALUES (
    (SELECT id FROM users WHERE username = 'student_demo'),
    (SELECT id FROM learners WHERE code = 'demo-learner')
);

INSERT OR IGNORE INTO teacher_course_assignments (
    user_id,
    course_id
)
VALUES (
    (SELECT id FROM users WHERE username = 'teacher_demo'),
    (SELECT id FROM courses WHERE code = 'data-structures')
);
