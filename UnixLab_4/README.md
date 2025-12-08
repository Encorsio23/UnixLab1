
### API Документация для `/check_prime`

#### Описание

Этот API проверяет, является ли переданное число простым.

#### Метод

`POST`

#### URL

`http://localhost:5000/check_prime`

#### Заголовки

- `Content-Type: application/json` — Указывает, что данные отправляются в формате JSON.
    

#### Тело запроса

Тело запроса должно содержать JSON-объект с одним полем:

```json
{
  "number": <число>
}
```

Где:

- `number` — целое число для проверки на простоту.
    

#### Пример запроса

```bash
curl -X POST http://localhost:5000/check_prime -H "Content-Type: application/json" -d '{"number": 17}'
```

#### Ответ

##### Успешный ответ (HTTP Status 200)

Если запрос прошел успешно и число простое:

```json
{
  "result": "prime"
}
```

Если число не простое:

```json
{
  "result": "not_prime"
}
```

##### Ошибка (HTTP Status 400)

Если тело запроса неверное или отсутствует параметр `number`:

```json
{
  "error": "Invalid request, 'number' is required."
}
```

---

### Пример документации для ответа с ошибкой

```json
{
  "error": "Invalid request, 'number' is required."
}
```

### Пример ответа при успешной проверке

Для числа 17:

```json
{
  "result": "prime"
}
```

Для числа 18:

```json
{
  "result": "not_prime"
}
```
