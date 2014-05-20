CREATE TABLE u_users (
    id            SERIAL PRIMARY KEY,
    created  timestamp DEFAULT now(),
    modified timestamp DEFAULT now(),
    nicename varchar(255),
    username varchar UNIQUE NOT NULL,
    pass  varchar,
    email varchar(255)
);

CREATE TABLE u_posts (
    id   SERIAL PRIMARY KEY,
    parent_id INTEGER REFERENCES u_posts(id) DEFAULT NULL,
    user_id INTEGER REFERENCES u_users(id),
    created  timestamp DEFAULT now(),
    modified timestamp DEFAULT now(),
    title varchar,
    content text,
    comments_enabled boolean DEFAULT true
);
