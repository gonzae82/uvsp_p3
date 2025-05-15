create table sensores (
  id bigint generated always as identity primary key,
  temperatura float,
  umidade float,
  timestamp timestamptz default current_timestamp
);