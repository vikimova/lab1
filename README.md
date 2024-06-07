[![CI/CD GitHub Actions](https://github.com/vikimova/lab1/actions/workflows/test-workflow.yml/badge.svg)](https://github.com/vikimova/lab1/actions/workflows/test-workflow.yml)
[![Coverage Status](https://coveralls.io/repos/github/vikimova/lab1/badge.svg?branch=main)](https://coveralls.io/github/vikimova/lab1?branch=main)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=vikimova_1&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=vikimova_1)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=vikimova_1&metric=bugs)](https://sonarcloud.io/summary/new_code?id=vikimova_1)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=vikimova_1&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=vikimova_1)
# Автор:
Виктория Ефимова

# Проект:
## Пользователь может передвигаться по карте
### Сценарий работы:
1. Пользователь нажимает клавишу стрелки (Вверх, Вниз, Вправо или Влево)
2. Обновляется позиция игрока:

    2.1. Если на новой позиции нет стены - переместить игрока

    2.2. Если на новой есть стена - не перемещать игрока
   
## Враги могут передвигаться по карте
### Сценарий работы:
1. Генерация случайного хода врага на соседнюю позицию:

    1.1. Если на новой позиции нет стены - переместить врага

    1.2. Если на новой есть стена - не перемещать врага

2. Отрисовка карты

## Пользователь может получать урон
### Сценарий работы:
1. Пользователь переместился на новую позицию:

    1.1. На новой позиции есть враг:

    1.1.1. Переместить врага на прошлую позицию игрока

    1.1.2. Снизить здоровье игрока

2. Здоровье равно 0:

    2.1. Показать уведомление о проигрыше

    2.2. Выйти из игры

## Пользователь может войти в дверь
### Сценарий работы:
1. Пользователь переместился на новую позицию:

    1.1. На новой позиции есть дверь:

    1.1.1. Показать уведомление о победе

    1.1.2. Выйти из игры

## Пользователь может победить:
### Сценарий работы:
1. Пользователь вошел в дверь:
    
    1.1. Показать уведомление о победе

    1.2. Выйти из игры


## Пользователь может проиграть:
### Сценарий работы:
1. Пользователь лишился здоровья:
    
    1.1. Показать уведомление о проигрыше

    1.2. Выйти из игры

3. Отрисовка карты
# Сборка
```bash
$ cd game
$ make
$ make run
```

[Функциональные модели](./docs/functions.md) 

[Структурные модели](./docs/struct.md) 

[Поведенческие модели](./docs/behavior.md)

[Тестирование](./docs/tests.md) 
