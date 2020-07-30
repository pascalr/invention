CREATE TABLE COMPANY(
      ID INT PRIMARY KEY     NOT NULL,
      NAME           TEXT    NOT NULL,
      AGE            INT     NOT NULL,
      ADDRESS        CHAR(50),
      SALARY         REAL );

CREATE TABLE ALIMENT(
      ID INT PRIMARY KEY         NOT NULL,
      NAME           CHAR(50)    NOT NULL );

CREATE TABLE INGREDIENT(
      NAME           CHAR(50) PRIMARY KEY     NOT NULL,
      ALIMENT_ID     INT );
