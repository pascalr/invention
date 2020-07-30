CREATE TABLE COMPANY(
      ID INT PRIMARY KEY     NOT NULL,
      NAME           TEXT    NOT NULL,
      AGE            INT     NOT NULL,
      ADDRESS        CHAR(50),
      SALARY         REAL );

CREATE TABLE aliment(
      ID INT PRIMARY KEY         NOT NULL,
      NAME           CHAR(50)    NOT NULL );

CREATE TABLE ingredient(
      NAME           CHAR(50) PRIMARY KEY     NOT NULL,
      ALIMENT_ID     INT );
