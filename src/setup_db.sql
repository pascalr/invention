CREATE TABLE COMPANY(
      ID INT PRIMARY KEY     NOT NULL,
      NAME           TEXT    NOT NULL,
      AGE            INT     NOT NULL,
      ADDRESS        CHAR(50),
      SALARY         REAL );

CREATE TABLE recette(
      NAME           VARCHAR     NOT NULL,
      INSTRUCTIONS   VARCHAR     );

CREATE TABLE aliment(
      ID INT PRIMARY KEY         NOT NULL,
      NAME           VARCHAR     NOT NULL);

CREATE TABLE ingredient(
      NAME           VARCHAR     NOT NULL,
      ALIMENT_ID     INT );

CREATE TABLE jar(
      POS_X          REAL        NOT NULL,
      POS_Y          REAL        NOT NULL,
      POS_T          REAL        NOT NULL);

CREATE TABLE shelf(
      HEIGHT           REAL      NOT NULL,
      WIDTH            REAL      NOT NULL,
      DEPTH            REAL      NOT NULL,
      IS_WORKING_SHELF INTEGER   NOT NULL);

INSERT INTO shelf VALUES( 0.0, 640.0, 300.0, 0 );
INSERT INTO shelf VALUES( 675.0, 640.0, 300.0, 1 );
